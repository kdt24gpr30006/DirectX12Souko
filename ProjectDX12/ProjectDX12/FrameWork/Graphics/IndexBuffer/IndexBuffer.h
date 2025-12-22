#pragma once
#include <d3d12.h>
#include <stdint.h>

/// <summary>
/// インデックスバッファのフォーマット
/// </summary>
enum class EIndexBufferFormat : uint8_t
{
	Uint32,
	Uint16,
};


/// <summary>
/// インデックスバッファ用クラス
/// </summary>
class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	/// <summary>
	/// インデックスバッファの作成
	/// </summary>
	/// <param name="size">バッファサイズ</param>
	/// <returns></returns>
	bool Create(const size_t size);

	/// <summary>
	/// バッファの解放
	/// </summary>
	void Release();

	/// <summary>
	/// バッファの更新
	/// </summary>
	/// <param name="SrcData">更新データ</param>
	void Update(const void* SrcData);

	/// <summary>
	/// バッファの更新
	/// </summary>
	/// <param name="SrcData">更新データ</param>
	/// <param name="offset">オフセット値</param>
	/// <param name="size">更新サイズ</param>
	void Update(const void* SrcData, const size_t offset, const size_t size);

	/// <summary>
	/// インデックスバッファをパイプラインへセット
	/// </summary>
	void Set()const;

	void Set(const EIndexBufferFormat Format, const size_t offset, const size_t size)const;

	/// <summary>
	/// バッファサイズの取得
	/// </summary>
	/// <returns></returns>
	size_t GetSize()const;
private:
	/// <summary>
	/// インデックスバッファリソース
	/// </summary>
	ID3D12Resource* BufferResource;

	/// <summary>
	/// インデックスバッファの先頭アドレス
	/// </summary>
	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;

	/// <summary>
	/// バッファのサイズ
	/// </summary>
	size_t BufferSize;
};

