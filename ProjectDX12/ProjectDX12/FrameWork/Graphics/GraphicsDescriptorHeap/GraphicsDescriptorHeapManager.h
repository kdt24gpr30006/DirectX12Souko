#pragma once
#include <d3d12.h>

struct DescriptorHeapInfo
{
	int Index;
	int Size;
};

class GraphicsDescriptorHeapManager
{
	static constexpr int MaxDescriptor = 512;
public:

	static bool Create();
	static void Release();
	static GraphicsDescriptorHeapManager* GetInstance();

	bool Initialize();

	/// <summary>
	/// ヒープから使用領域の発行
	/// </summary>
	/// <param name="Size"></param>
	/// <returns></returns>
	DescriptorHeapInfo Issuance(int Size);

	/// <summary>
	/// 使用領域の破棄
	/// </summary>
	/// <param name="Index"></param>
	/// <param name="Size"></param>
	void Discard(DescriptorHeapInfo& descriptorHeapInfo);

	void Set(int RootParameterIndex, const DescriptorHeapInfo& descriptorHeapInfo);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(const DescriptorHeapInfo& descriptorHeapInfo)const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(const DescriptorHeapInfo& descriptorHeapInfo)const;

	/// <summary>
	/// グラフィックス用ディスクリプターヒープ
	/// </summary>
	ID3D12DescriptorHeap* GetGraphicsHeap();

private:

	struct HandleInfo
	{
		D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
	};

	static GraphicsDescriptorHeapManager* Instance;

	/// <summary>
	/// グラフィックス用ディスクリプターヒープ
	/// </summary>
	ID3D12DescriptorHeap* GraphicsHeap;

	HandleInfo Info[MaxDescriptor];

	bool bUseFlags[MaxDescriptor];

	int Offset;

private:

	GraphicsDescriptorHeapManager();
	~GraphicsDescriptorHeapManager();
	GraphicsDescriptorHeapManager(const GraphicsDescriptorHeapManager&) = delete;
	GraphicsDescriptorHeapManager(GraphicsDescriptorHeapManager&&) = delete;
	GraphicsDescriptorHeapManager& operator=(const GraphicsDescriptorHeapManager&) = delete;
	GraphicsDescriptorHeapManager& operator=(GraphicsDescriptorHeapManager&&) = delete;
};

