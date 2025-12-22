#include "GraphicsDescriptorHeap.h"
#include "../DirectX/DirectX.h"

GraphicsDescriptorHeap::GraphicsDescriptorHeap()
	: HeapInfo()
{
}
GraphicsDescriptorHeap::~GraphicsDescriptorHeap()
{
	Release();
}

/// <summary>
/// ヒープの作成
/// </summary>
/// <returns>true:成功</returns>
bool GraphicsDescriptorHeap::Create()
{
	GraphicsDescriptorHeapManager* HeapManager = GraphicsDescriptorHeapManager::GetInstance();
	HeapInfo = HeapManager->Issuance(1);
	return true;
}

/// <summary>
/// ヒープの解放
/// </summary>
void GraphicsDescriptorHeap::Release()
{
	GraphicsDescriptorHeapManager* HeapManager = GraphicsDescriptorHeapManager::GetInstance();
	HeapManager->Discard(HeapInfo);
}

/// <summary>
/// パイプラインへヒープのセット
/// </summary>
void GraphicsDescriptorHeap::Set(int RootParameterIndex)
{
	GraphicsDescriptorHeapManager* HeapManager = GraphicsDescriptorHeapManager::GetInstance();
	HeapManager->Set(RootParameterIndex, HeapInfo);

}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetCpuHandle()const
{
	GraphicsDescriptorHeapManager* HeapManager = GraphicsDescriptorHeapManager::GetInstance();
	return HeapManager->GetCpuHandle(HeapInfo);

}
D3D12_GPU_DESCRIPTOR_HANDLE GraphicsDescriptorHeap::GetGpuHandle()const
{
	GraphicsDescriptorHeapManager* HeapManager = GraphicsDescriptorHeapManager::GetInstance();
	return HeapManager->GetGpuHandle(HeapInfo);

}