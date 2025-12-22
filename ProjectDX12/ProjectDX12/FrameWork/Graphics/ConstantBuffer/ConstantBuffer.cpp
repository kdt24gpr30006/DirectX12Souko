#include "ConstantBuffer.h"
#include "../GraphicsDescriptorHeap/GraphicsDescriptorHeap.h"
#include "../DirectX/DirectX.h"

ConstantBuffer::ConstantBuffer()
	: BufferResource(nullptr)
	, Heap(nullptr)
	, BufferSize(0)
	, AlignedBufferSize(0)
{
}
ConstantBuffer::~ConstantBuffer()
{
	Release();
}

/// <summary>
/// 定数バッファの作成
/// </summary>
/// <param name="Size">定数バッファのサイズ</param>
/// <returns>true:成功</returns>
bool ConstantBuffer::Create(size_t Size)
{
	BufferSize = static_cast<UINT64>(Size);
	//	256byte単位で作成されるよう算出
	AlignedBufferSize = (BufferSize + 0xff) & ~0xff;

	//	ヒープ設定
	D3D12_HEAP_PROPERTIES heapDesc = {};
	heapDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.CreationNodeMask = 1;
	heapDesc.VisibleNodeMask = 1;

	//	バッファのリソース
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = AlignedBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Device* device = Graphics::DirectX::GetInstance()->GetDevice();
	const HRESULT hr = device->CreateCommittedResource(
		&heapDesc,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&BufferResource));
	if (FAILED(hr))
	{
		return false;
	}

	Heap = new GraphicsDescriptorHeap();
	const bool ret = Heap->Create();
	if (ret == false)
	{
		SafeRelease(BufferResource);
		return false;
	}

	//	CBV作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBV = {};
	CBV.BufferLocation = BufferResource->GetGPUVirtualAddress();
	CBV.SizeInBytes = static_cast<UINT>(AlignedBufferSize);
	const D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle = Heap->GetCpuHandle();
	device->CreateConstantBufferView(&CBV, CpuHandle);

	return true;
}

/// <summary>
/// バッファの解放
/// </summary>
void ConstantBuffer::Release()
{
	SafeRelease(BufferResource);
	if (Heap != nullptr)
	{
		Heap->Release();
		delete Heap;
		Heap = nullptr;
	}
}

/// <summary>
/// バッファの更新
/// </summary>
/// <param name="SrcData"></param>
void ConstantBuffer::Update(const void* SrcData)
{
	D3D12_RANGE Range = {};
	Range.Begin = 0;
	Range.End = static_cast<SIZE_T>(BufferSize);

	void* pData = nullptr;
	const HRESULT hr = BufferResource->Map(0, &Range, &pData);
	if (SUCCEEDED(hr))
	{
		memcpy(pData, SrcData, Range.End);
		BufferResource->Unmap(0, &Range);
	}
}

/// <summary>
/// バッファをパイプラインへセット
/// </summary>
/// <param name="RootParameterIndex">ルートシグネチャへセットしたパラメーターインデックス</param>
void ConstantBuffer::Set(int RootParameterIndex)
{
	Heap->Set(RootParameterIndex);
}