#include "Texture.h"
#include <string>
#include <algorithm>
#include "../DirectX/DirectX.h"
#include "../GraphicsDescriptorHeap/GraphicsDescriptorHeap.h"

namespace DirectXTex = ::DirectX;

Texture::Texture()
	: TextureResorce(nullptr)
	, GraphicsHeap(nullptr)
	, Width()
	, Height()
{
}
Texture::~Texture()
{
	Release();
}

bool Texture::Create(const std::filesystem::path& FilePath)
{
    if (!std::filesystem::exists(FilePath))
    {
        return false;
    }

    // ===============================
    // 画像読み込み
    // ===============================
    DirectX::TexMetadata metaData = {};
    DirectX::ScratchImage scratch = {};

    HRESULT hr = S_FALSE;
    std::wstring path = FilePath.wstring();
    auto ext = FilePath.extension();

    if (ext == ".dds")
    {
        hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &metaData, scratch);
    }
    else if (ext == ".tga")
    {
        hr = DirectX::LoadFromTGAFile(path.c_str(), DirectX::TGA_FLAGS_NONE, &metaData, scratch);
    }
    else
    {
        hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, &metaData, scratch);
    }

    if (FAILED(hr))
    {
        return false;
    }

    const DirectX::Image* image = scratch.GetImage(0, 0, 0);

    // ===============================
    // Device 取得
    // ===============================
    Graphics::DirectX* dx = Graphics::DirectX::GetInstance();
    ID3D12Device* device = dx->GetDevice();

    // ===============================
    // テクスチャ ResourceDesc
    // ===============================
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = metaData.width;
    texDesc.Height = (UINT)metaData.height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = metaData.format;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // ===============================
    // Copy 用 Footprint 取得
    // ===============================
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
    UINT numRows = 0;
    UINT64 rowSize = 0;
    UINT64 totalBytes = 0;

    device->GetCopyableFootprints(
        &texDesc,
        0,
        1,
        0,
        &footprint,
        &numRows,
        &rowSize,
        &totalBytes
    );

    // ===============================
    // Upload Buffer 作成
    // ===============================
    D3D12_HEAP_PROPERTIES uploadHeap = {};
    uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC uploadDesc = {};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Width = totalBytes;
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadDesc.SampleDesc.Count = 1;

    ID3D12Resource* uploadBuffer = nullptr;
    hr = device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    );

    if (FAILED(hr))
    {
        return false;
    }

    // ===============================
    // Texture Resource 作成
    // ===============================
    D3D12_HEAP_PROPERTIES texHeap = {};
    texHeap.Type = D3D12_HEAP_TYPE_DEFAULT;

    hr = device->CreateCommittedResource(
        &texHeap,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&TextureResorce)
    );

    if (FAILED(hr))
    {
        uploadBuffer->Release();
        return false;
    }

    // ===============================
    // ★ 正しい RowPitch コピー処理
    // ===============================
    void* mapped = nullptr;
    uploadBuffer->Map(0, nullptr, &mapped);

    uint8_t* dst = reinterpret_cast<uint8_t*>(mapped);
    const uint8_t* src = image->pixels;

    // コピーする1行の安全サイズ
    const UINT copyBytes = static_cast<UINT>(
        min(
            static_cast<size_t>(image->rowPitch),
            static_cast<size_t>(footprint.Footprint.RowPitch)
        )
        );

    // GPUが要求する行数を使う
    for (UINT y = 0; y < numRows; ++y)
    {
        uint8_t* dstRow = dst + y * footprint.Footprint.RowPitch;

        // まず行全体を 0 クリア（超重要）
        memset(dstRow, 0, footprint.Footprint.RowPitch);

        // 有効な範囲だけコピー
        if (y < metaData.height)
        {
            memcpy(
                dstRow,
                src + y * image->rowPitch,
                copyBytes
            );
        }
    }

    uploadBuffer->Unmap(0, nullptr);

    // ===============================
    // CopyTextureRegion
    // ===============================
    D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
    srcLoc.pResource = uploadBuffer;
    srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLoc.PlacedFootprint = footprint;

    D3D12_TEXTURE_COPY_LOCATION dstLoc = {};
    dstLoc.pResource = TextureResorce;
    dstLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLoc.SubresourceIndex = 0;

    ID3D12GraphicsCommandList* cmd = dx->GetCommandList();
    cmd->CopyTextureRegion(&dstLoc, 0, 0, 0, &srcLoc, nullptr);

    // ===============================
    // Barrier
    // ===============================
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = TextureResorce;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    cmd->ResourceBarrier(1, &barrier);

    cmd->Close();
    ID3D12CommandList* lists[] = { cmd };
    dx->GetCommandQueue()->ExecuteCommandLists(1, lists);
    dx->WaitForGPU();

    dx->GetCommandAllocator()->Reset();
    cmd->Reset(dx->GetCommandAllocator(), nullptr);

    // ===============================
    // SRV 作成
    // ===============================
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = metaData.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    GraphicsHeap = new GraphicsDescriptorHeap();
    GraphicsHeap->Create();
    device->CreateShaderResourceView(
        TextureResorce,
        &srvDesc,
        GraphicsHeap->GetCpuHandle()
    );

    Width = (float)metaData.width;
    Height = (float)metaData.height;

    uploadBuffer->Release();
    return true;
}


void Texture::Release()
{
	SafeRelease(TextureResorce);
	if (GraphicsHeap != nullptr)
	{
		GraphicsHeap->Release();
		delete GraphicsHeap;
		GraphicsHeap = nullptr;
	}
}

/// <summary>
/// パイプラインへのセット
/// </summary>
/// <param name="RootParameterIndex"></param>
void Texture::Set(int RootParameterIndex)
{
	if (GraphicsHeap != nullptr)
	{
		GraphicsHeap->Set(RootParameterIndex);
	}
}

float Texture::GetWidth()const
{
	return Width;
}
float Texture::GetHeight()const
{
	return Height;
}

D3D12_RESOURCE_DESC Texture::GetTextureResourceDesc(const DirectX::TexMetadata& MetaData)
{
	D3D12_RESOURCE_DESC Desc = {};
	Desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(MetaData.dimension);
	Desc.Alignment = 0;
	Desc.Width = static_cast<UINT64>(MetaData.width);
	Desc.Height = static_cast<UINT>(MetaData.height);
	Desc.DepthOrArraySize = static_cast<UINT16>(MetaData.arraySize);
	Desc.MipLevels = 1;
	Desc.Format = MetaData.format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	return Desc;
}
