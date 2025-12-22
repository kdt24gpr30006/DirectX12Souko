#pragma once
#include <d3d12.h>
#include <filesystem>
#include <Plugin/DirectXTex/DirectXTex.h>

class GraphicsDescriptorHeap;

class Texture
{
public:
	Texture();
	~Texture();

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="FilePath">ファイルパス</param>
	/// <returns>true:成功</returns>
	bool Create(const std::filesystem::path& FilePath);

	/// <summary>
	/// 解放
	/// </summary>
	void Release();

	/// <summary>
	/// パイプラインへのセット
	/// </summary>
	/// <param name="RootParameterIndex"></param>
	void Set(int RootParameterIndex);

	/// <summary>
	/// テクスチャの幅
	/// </summary>
	/// <returns></returns>
	float GetWidth()const;
	/// <summary>
	/// テクスチャの高さ
	/// </summary>
	/// <returns></returns>
	float GetHeight()const;

private:

	/// <summary>
	/// 読み込んだテクスチャの情報取得
	/// </summary>
	/// <param name="MetaData"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC GetTextureResourceDesc(const DirectX::TexMetadata& MetaData);

	/// <summary>
	/// テクスチャ用リソース
	/// </summary>
	ID3D12Resource* TextureResorce;

	/// <summary>
	/// テクスチャ用ヒープ
	/// </summary>
	GraphicsDescriptorHeap* GraphicsHeap;

	/// <summary>
	/// テクスチャの幅
	/// </summary>
	float Width;
	/// <summary>
	/// テクスチャの高さ
	/// </summary>
	float Height;
};

