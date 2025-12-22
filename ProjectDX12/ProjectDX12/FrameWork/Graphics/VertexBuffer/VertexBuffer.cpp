#include "VertexBuffer.h"
#include "../DirectX/DirectX.h"

VertexBuffer::VertexBuffer()
	: BufferResource(nullptr)
	, BufferLocation()
	, Stride()
	, BufferSize()
{
}
VertexBuffer::~VertexBuffer()
{
	Release();
}

/// <summary>
/// 頂点バッファの作成
/// </summary>
/// <param name="Size">バッファのサイズ</param>
/// <param name="Stride">1頂点のデータサイズ</param>
bool VertexBuffer::Create(const size_t size, const size_t stride)
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
	resourceDesc.Width = static_cast<UINT64>(size);
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
	BufferSize = size;
	Stride = stride;

	return true;
}

/// <summary>
/// バッファの解放
/// </summary>
void VertexBuffer::Release()
{
	SafeRelease(BufferResource);
	BufferLocation = 0;
	BufferSize = 0;
	Stride = 0;
}

/// <summary>
/// バッファの更新
/// </summary>
/// <param name="SrcData">更新データ</param>
void VertexBuffer::Update(const void* SrcData)
{
	Update(SrcData, 0, GetSize());
}

/// <summary>
/// バッファの更新
/// </summary>
/// <param name="SrcData">更新データ<</param>
/// <param name="offset">オフセット値</param>
/// <param name="size">更新サイズ</param>
void VertexBuffer::Update(const void* SrcData, const size_t offset, const size_t size)
{
	D3D12_RANGE Range = {};
	Range.Begin = static_cast<SIZE_T>(offset);
	Range.End = Range.Begin + size;

	uint8_t* pData = nullptr;
	const HRESULT hr = BufferResource->Map(0, &Range, reinterpret_cast<void**>(&pData));
	if (SUCCEEDED(hr))
	{
		memcpy(pData + offset, SrcData, size);
		BufferResource->Unmap(0, &Range);
	}
}

/// <summary>
/// 頂点バッファをパイプラインへセット
/// </summary>
void VertexBuffer::Set()const
{
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = BufferLocation;
	vbv.SizeInBytes = static_cast<UINT>(BufferSize);
	vbv.StrideInBytes = static_cast<UINT>(Stride);
	ID3D12GraphicsCommandList* comandlist = Graphics::DirectX::GetInstance()->GetCommandList();
	comandlist->IASetVertexBuffers(0, 1, &vbv);
}

void VertexBuffer::Set(const size_t offset, const size_t stride, const size_t size)const
{
	D3D12_VERTEX_BUFFER_VIEW vbv = {};
	vbv.BufferLocation = BufferLocation + static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(offset);
	vbv.SizeInBytes = static_cast<UINT>(size);
	vbv.StrideInBytes = static_cast<UINT>(stride);
	ID3D12GraphicsCommandList* comandlist = Graphics::DirectX::GetInstance()->GetCommandList();
	comandlist->IASetVertexBuffers(0, 1, &vbv);
}

/// <summary>
/// 頂点バッファのサイズの取得
/// </summary>
/// <returns></returns>
size_t VertexBuffer::GetSize() const
{
	return BufferSize;
}