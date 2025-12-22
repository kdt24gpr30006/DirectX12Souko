#include "IndexBuffer.h"
#include "Graphics/DirectX/DirectX.h"
#include "System/Debug/Debug.h" 

IndexBuffer::IndexBuffer()
	: BufferResource(nullptr)
	, BufferLocation()
	, BufferSize()
{
}
IndexBuffer::~IndexBuffer()
{
	Release();
}

/// <summary>
/// インデックスバッファの作成
/// </summary>
/// <param name="mode">バッファ用フォーマットの指定</param>
/// <param name="DataSize">バッファサイズ</param>
/// <returns></returns>
bool IndexBuffer::Create(const size_t DataSize)
{
	//	ヒープ設定
	D3D12_HEAP_PROPERTIES heapDesc = {};
	heapDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDesc.CreationNodeMask = 0;
	heapDesc.VisibleNodeMask = 0;

	//	バッファのリソース
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = static_cast<UINT64>(DataSize);
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Device* device = Graphics::DirectX::GetInstance()->GetDevice();
	const HRESULT hr = device->CreateCommittedResource(&heapDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&BufferResource));
	if (FAILED(hr))
	{
		return false;
	}
	
	BufferLocation = BufferResource->GetGPUVirtualAddress();
	BufferSize = DataSize;

	return true;
}

/// <summary>
/// バッファの解放
/// </summary>
void IndexBuffer::Release()
{
	SafeRelease(BufferResource);
}

/// <summary>
/// バッファの更新
/// </summary>
/// <param name="SrcData">更新データ</param>
void IndexBuffer::Update(const void* SrcData)
{
	Update(SrcData, 0, GetSize());
}

/// <summary>
/// バッファの更新
/// </summary>
/// <param name="SrcData">更新データ</param>
/// <param name="offset">オフセット値</param>
/// <param name="size">更新サイズ</param>
void IndexBuffer::Update(const void* SrcData, const size_t offset, const size_t size)
{
	D3D12_RANGE Range = {};
	Range.Begin = static_cast<SIZE_T>(offset);
	Range.End = Range.Begin + static_cast<SIZE_T>(size);

	uint8_t* pData = nullptr;
	const HRESULT hr = BufferResource->Map(0, &Range, reinterpret_cast<void**>(&pData));
	if (SUCCEEDED(hr))
	{
		memcpy(pData + offset, SrcData, size);
		BufferResource->Unmap(0, &Range);
	}
}

/// <summary>
/// インデックスバッファをパイプラインへセット
/// </summary>
void IndexBuffer::Set()const
{
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = BufferLocation;
	ibv.SizeInBytes = static_cast<UINT>(BufferSize);
	ibv.Format = DXGI_FORMAT_R32_UINT;
	ID3D12GraphicsCommandList* comandlist = Graphics::DirectX::GetInstance()->GetCommandList();
	comandlist->IASetIndexBuffer(&ibv);
}

void IndexBuffer::Set(const EIndexBufferFormat Format, const size_t offset, const size_t size)const
{
	D3D12_INDEX_BUFFER_VIEW ibv = {};
	ibv.BufferLocation = BufferLocation + static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(offset);
	ibv.SizeInBytes = static_cast<UINT>(size);
	switch (Format)
	{
	case EIndexBufferFormat::Uint32:
		ibv.Format = DXGI_FORMAT_R32_UINT;
		break;
	case EIndexBufferFormat::Uint16:
		ibv.Format = DXGI_FORMAT_R16_UINT;
		break;
	default:
		DEBUG_LOG(ELogLevel::Error, "Invalid IndexBufferFormat.\n");
		break;
	}
	ID3D12GraphicsCommandList* comandlist = Graphics::DirectX::GetInstance()->GetCommandList();
	comandlist->IASetIndexBuffer(&ibv);
}

/// <summary>
/// バッファサイズの取得
/// </summary>
/// <returns></returns>
size_t IndexBuffer::GetSize()const
{
	return BufferSize;
}