#pragma once
#include <d3d12.h>

class GraphicsDescriptorHeap;

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	/// <summary>
	/// 定数バッファの作成
	/// </summary>
	/// <param name="Size">定数バッファのサイズ</param>
	/// <returns>true:成功</returns>
	bool Create(size_t Size);

	/// <summary>
	/// バッファの解放
	/// </summary>
	void Release();

	/// <summary>
	/// バッファの更新
	/// </summary>
	/// <param name="SrcData"></param>
	void Update(const void* SrcData);

	/// <summary>
	/// バッファをパイプラインへセット
	/// </summary>
	/// <param name="RootParameterIndex">ルートシグネチャへセットしたパラメーターインデックス</param>
	void Set(int RootParameterIndex);

private:
	/// <summary>
	/// 定数バッファリソース
	/// </summary>
	ID3D12Resource* BufferResource;

	GraphicsDescriptorHeap* Heap;

	UINT64 BufferSize;
	UINT64 AlignedBufferSize;
};

