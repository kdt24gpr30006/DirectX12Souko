#pragma once
#include <d3d12.h>
#include <stdint.h>

//	頂点バッファ
class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	/// <summary>
	/// 頂点バッファの作成
	/// </summary>
	/// <param name="size">バッファのサイズ</param>
	/// <param name="stride">1頂点のデータサイズ</param>
	/// <returns></returns>
	bool Create(const size_t size, const size_t stride);

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
	/// <param name="SrcData">更新データ<</param>
	/// <param name="offset">オフセット値</param>
	/// <param name="size">更新サイズ</param>
	void Update(const void* SrcData, const size_t offset, const size_t size);

	/// <summary>
	/// 頂点バッファをパイプラインへセット
	/// </summary>
	void Set()const;

	void Set(const size_t offset, const size_t stride, const size_t size)const;

	/// <summary>
	/// 頂点バッファのサイズの取得
	/// </summary>
	/// <returns></returns>
	size_t GetSize()const;

private:
	/// <summary>
	/// 頂点バッファリソース
	/// </summary>
	ID3D12Resource* BufferResource;

	/// <summary>
	/// 頂点バッファの先頭アドレス
	/// </summary>
	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;

	/// <summary>
	/// 1頂点のサイズ
	/// </summary>
	size_t Stride;

	/// <summary>
	/// 頂点バッファのサイズ
	/// </summary>
	size_t BufferSize;
};
