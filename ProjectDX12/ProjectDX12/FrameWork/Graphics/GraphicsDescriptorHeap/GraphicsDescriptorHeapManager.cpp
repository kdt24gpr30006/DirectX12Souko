#include "GraphicsDescriptorHeapManager.h"
#include "../DirectX/DirectX.h"

GraphicsDescriptorHeapManager* GraphicsDescriptorHeapManager::Instance = nullptr;

GraphicsDescriptorHeapManager::GraphicsDescriptorHeapManager()
	: GraphicsHeap(nullptr)
	, Info()
	, bUseFlags()
	, Offset(0)
{
}
GraphicsDescriptorHeapManager::~GraphicsDescriptorHeapManager()
{
	SafeRelease(GraphicsHeap);
}

bool GraphicsDescriptorHeapManager::Create()
{
	if (Instance == nullptr)
	{
		Instance = new GraphicsDescriptorHeapManager();
		return true;
	}
	return false;
}

void GraphicsDescriptorHeapManager::Release()
{
	if (Instance != nullptr)
	{
		delete Instance;
		Instance = nullptr;
	}
}

GraphicsDescriptorHeapManager* GraphicsDescriptorHeapManager::GetInstance()
{
	return Instance;
}

bool GraphicsDescriptorHeapManager::Initialize()
{
	ID3D12Device* Device = Graphics::DirectX::GetInstance()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
	HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	HeapDesc.NumDescriptors = MaxDescriptor;
	HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HeapDesc.NodeMask = 0;
	const HRESULT hr = Device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&GraphicsHeap));
	if (FAILED(hr))
	{
		return false;
	}

	const D3D12_CPU_DESCRIPTOR_HANDLE CpuStart = GraphicsHeap->GetCPUDescriptorHandleForHeapStart();
	const D3D12_GPU_DESCRIPTOR_HANDLE GpuStart = GraphicsHeap->GetGPUDescriptorHandleForHeapStart();
	const SIZE_T IncrementSize = static_cast<SIZE_T>(Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	SIZE_T off = 0;
	//	全てのハンドルを初期化
	for (auto& itr : Info)
	{
		itr.CpuHandle = CpuStart;
		itr.CpuHandle.ptr += off;

		itr.GpuHandle = GpuStart;
		itr.GpuHandle.ptr += off;

		off += IncrementSize;
	}

	for (auto& itr : bUseFlags)
	{
		//	一応明示的に初期化
		itr = false;
	}

	return true;
}

/// <summary>
/// ヒープから使用領域の発行
/// </summary>
/// <param name="Size"></param>
/// <returns></returns>
DescriptorHeapInfo GraphicsDescriptorHeapManager::Issuance(int Size)
{
	const int lastIndex = Offset + Size;
	if (lastIndex >= MaxDescriptor)
	{
		//	連続した使用領域を確保出来ないので0からチェックする
		Offset = 0;
	}
	//	空き領域の検索
	for (int count = 0; count < MaxDescriptor; count++, Offset++)
	{
		if (bUseFlags[Offset] == true)
		{
			//	使用中
			continue;
		}
		
		//	使用領域の連続してるかチェック
		bool bCheck = true;
		for (int s = Offset + 1; s < Offset + Size; s++)
		{
			if (bUseFlags[s] == true)
			{
				bCheck = false;
				break;
			}
		}
		if (bCheck == false)
		{
			continue;
		}

		//	使用可能領域の確保
		for (int off = Offset; off < Offset + Size; off++)
		{
			bUseFlags[off] = true;
		}
		DescriptorHeapInfo info = {};
		info.Index = Offset;
		info.Size = Size;
		return info;
	}

	//	TODO:ここに来るとアプリを停止させる(継続して実行不可)
	const DescriptorHeapInfo ret = { -1, -1 };
	return ret;
}

/// <summary>
/// 使用領域の破棄
/// </summary>
/// <param name="Index"></param>
/// <param name="Size"></param>
void GraphicsDescriptorHeapManager::Discard(DescriptorHeapInfo& descriptorHeapInfo)
{
	if (descriptorHeapInfo.Index < 0)
	{
		return;
	}

	const int lastIndex = descriptorHeapInfo.Index + descriptorHeapInfo.Size;
	for (int index = descriptorHeapInfo.Index; index < lastIndex; index++)
	{
		bUseFlags[index] = false;
	}

	descriptorHeapInfo.Index = -1;
	descriptorHeapInfo.Size = -1;
}

void GraphicsDescriptorHeapManager::Set(int RootParameterIndex, const DescriptorHeapInfo& descriptorHeapInfo)
{
	if (descriptorHeapInfo.Index < 0)
	{
		return;
	}
	ID3D12GraphicsCommandList* CommandList = Graphics::DirectX::GetInstance()->GetCommandList();

	CommandList->SetDescriptorHeaps(1, &GraphicsHeap);
	const UINT index = static_cast<int>(RootParameterIndex);
	CommandList->SetGraphicsRootDescriptorTable(index, Info[descriptorHeapInfo.Index].GpuHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeapManager::GetCpuHandle(const DescriptorHeapInfo& descriptorHeapInfo)const
{
	if (descriptorHeapInfo.Index < 0)
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
	return Info[descriptorHeapInfo.Index].CpuHandle;
}
D3D12_GPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeapManager::GetGpuHandle(const DescriptorHeapInfo& descriptorHeapInfo)const
{
	if (descriptorHeapInfo.Index < 0)
	{
		return D3D12_GPU_DESCRIPTOR_HANDLE();
	}
	return Info[descriptorHeapInfo.Index].GpuHandle;
}

/// <summary>
/// グラフィックス用ディスクリプターヒープ
/// </summary>
ID3D12DescriptorHeap* GraphicsDescriptorHeapManager::GetGraphicsHeap()
{
	return GraphicsHeap; 
}