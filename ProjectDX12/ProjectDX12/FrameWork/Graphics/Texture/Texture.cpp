#include "Texture.h"
#include <string>
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
	if (std::filesystem::exists(FilePath) == false)
	{
		return false;
	}
	//	「.xxx」
	const std::filesystem::path Extension = FilePath.extension();

	DirectXTex::TexMetadata TexMetaData = {};
	DirectXTex::ScratchImage ScratchImage = {};

	HRESULT hr = S_FALSE;
	std::wstring Path = FilePath.wstring();
	if (Extension.compare(".dds") == 0)
	{
		hr = DirectXTex::LoadFromDDSFile(
			Path.c_str(),
			DirectXTex::DDS_FLAGS_NONE,
			&TexMetaData,
			ScratchImage);
	}
	else if (Extension.compare(".tga") == 0)
	{
		hr = DirectXTex::LoadFromTGAFile(
			Path.c_str(),
			DirectXTex::TGA_FLAGS_NONE,
			&TexMetaData,
			ScratchImage);
	}
	else if (Extension.compare(".hdr") == 0)
	{
		hr = DirectXTex::LoadFromHDRFile(
			Path.c_str(),
			&TexMetaData,
			ScratchImage);
	}
	else
	{
		hr = DirectXTex::LoadFromWICFile(
			Path.c_str(), 
			DirectXTex::WIC_FLAGS_NONE,
			&TexMetaData,
			ScratchImage);
	}
	if (FAILED(hr))
	{
		return false;
	}
	const DirectXTex::Image* Image = ScratchImage.GetImage(0, 0, 0);

	Graphics::DirectX* DirectXInstance = Graphics::DirectX::GetInstance();
	ID3D12Device* Device = DirectXInstance->GetDevice();

	//	読み込んだテクスチャからコピー用情報の取得
	const D3D12_RESOURCE_DESC TextureResourceDesc = GetTextureResourceDesc(TexMetaData);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footpoint = {};
	UINT row = 0;
	UINT64 rowsizeinbyte = 0;
	UINT64 totalbyte = 0;
	Device->GetCopyableFootprints(
		&TextureResourceDesc, 0, 1, 0,
		&footpoint, &row, &rowsizeinbyte, &totalbyte);


	//	アップロード用中間バッファ作成
	D3D12_HEAP_PROPERTIES UploadProperty = {};
	UploadProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	UploadProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	UploadProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	UploadProperty.CreationNodeMask = 0;
	UploadProperty.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC UploadResourceDesc = {};
	UploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	UploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	UploadResourceDesc.Width = totalbyte;
	UploadResourceDesc.Height = 1;
	UploadResourceDesc.DepthOrArraySize = 1;
	UploadResourceDesc.MipLevels = 1;
	UploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	UploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	UploadResourceDesc.SampleDesc.Count = 1;
	UploadResourceDesc.SampleDesc.Quality = 0;

	ID3D12Resource* UploadBuffer = nullptr;
	hr = Device->CreateCommittedResource(
		&UploadProperty, D3D12_HEAP_FLAG_NONE, &UploadResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&UploadBuffer));
	if (FAILED(hr))
	{
		return false;
	}

	//	ヒープ設定
	D3D12_HEAP_PROPERTIES HeapProperty = {};
	HeapProperty.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperty.CreationNodeMask = 0;
	HeapProperty.VisibleNodeMask = 0;

	//	テクスチャ用バッファ作成
	hr = Device->CreateCommittedResource(
		&HeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&TextureResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&TextureResorce));
	if (FAILED(hr))
	{
		return false;
	}


	/////////////////////////////////行ごとにコピーに帰る
	void* Data = nullptr;
	hr = UploadBuffer->Map(0, nullptr, &Data);
	if (SUCCEEDED(hr))
	{
		const size_t size = Image->slicePitch;
		memcpy(Data, ScratchImage.GetPixels(), size);
		UploadBuffer->Unmap(0, nullptr);
	}
	
	//	コピー元バッファ設定(アップロード側)
	D3D12_TEXTURE_COPY_LOCATION Src = {};
	Src.pResource = UploadBuffer;
	Src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	Src.PlacedFootprint = footpoint;

	//	コピー先バッファ設定(テクスチャとして参照する側)
	D3D12_TEXTURE_COPY_LOCATION Dest = {};
	Dest.pResource = TextureResorce;
	Dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	Dest.SubresourceIndex = 0;

	ID3D12GraphicsCommandList* CommandList = DirectXInstance->GetCommandList();
	//	バッファのコピー
	CommandList->CopyTextureRegion(&Dest, 0, 0, 0, &Src, nullptr);
	//	バリアー処理
	D3D12_RESOURCE_BARRIER Barrier = {};
	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	Barrier.Transition.pResource = TextureResorce;
	Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	CommandList->ResourceBarrier(1, &Barrier);
	CommandList->Close();

	//	コマンドリスト実行
	ID3D12CommandList* cmdLists[] = { CommandList };
	ID3D12CommandQueue* CommandQueue = DirectXInstance->GetCommandQueue();
	CommandQueue->ExecuteCommandLists(1, cmdLists);
	//	実行中なのでフェンス待ち
	DirectXInstance->WaitForGPU();
	ID3D12CommandAllocator* CommandAllocater = DirectXInstance->GetCommandAllocator();
	CommandAllocater->Reset();
	CommandList->Reset(CommandAllocater, nullptr);

	//	シェーダーリソース作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = TexMetaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	GraphicsHeap = new GraphicsDescriptorHeap();
	GraphicsHeap->Create();
	const D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle = GraphicsHeap->GetCpuHandle();
	Device->CreateShaderResourceView(TextureResorce, &srvDesc, CpuHandle);

	Width = static_cast<float>(TexMetaData.width);
	Height = static_cast<float>(TexMetaData.height);

	SafeRelease(UploadBuffer);
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
	Desc.MipLevels = static_cast<UINT16>(MetaData.mipLevels);
	Desc.Format = MetaData.format;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	return Desc;
}
