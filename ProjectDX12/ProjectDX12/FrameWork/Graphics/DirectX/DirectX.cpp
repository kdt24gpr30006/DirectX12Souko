#include "DirectX.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

namespace Graphics
{
	DirectX* DirectX::Instance = nullptr;

	DirectX::DirectX()
		: Device(nullptr)
		, Factory(nullptr)
		, SwapChain(nullptr)
		, CommandAllocator(nullptr)
		, CommandList(nullptr)
		, CommandQueue(nullptr)
		, BackBuffers()
		, DepthBuffer(nullptr)
		, BackBufferHeap(nullptr)
		, DepthHeap(nullptr)
		, Fence(nullptr)
		, DebugDevice(nullptr)
		, FenceValue()
		, ResourceBarrier()
		, WaitForGPUEventHandle(nullptr)
	{
	}
	DirectX::~DirectX()
	{
		WaitForGPU();

		CloseHandle(WaitForGPUEventHandle);

		for (auto& Buffer : BackBuffers)
		{
			SafeRelease(Buffer);
		}
		SafeRelease(DepthBuffer);
		SafeRelease(BackBufferHeap);
		SafeRelease(DepthHeap);

		SafeRelease(CommandAllocator);
		SafeRelease(CommandList);
		SafeRelease(CommandQueue);
		SafeRelease(SwapChain);

		SafeRelease(Fence);
		SafeRelease(Factory);

		SafeRelease(Device);
		if (DebugDevice != nullptr)
		{
			DebugDevice->Release();
			DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
			DebugDevice = nullptr;
		}
	}

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns></returns>
	bool DirectX::Create()
	{
		if (Instance == nullptr)
		{
			Instance = new DirectX();
			return true;
		}
		return false;
	}
	/// <summary>
	/// インスタンスの解放
	/// </summary>
	void DirectX::Release()
	{
		if (Instance != nullptr)
		{
			delete Instance;
			Instance = nullptr;
		}
	}
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	DirectX* DirectX::GetInstance()
	{
		return Instance;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="WindowHandle">ウィンドウのハンドル</param>
	/// <param name="Width">スクリーン横幅</param>
	/// <param name="Height">スクリーン縦幅</param>
	/// <returns>true:成功</returns>
	bool DirectX::Init(HWND WindowHandle, UINT Width, UINT Height)
	{
		bool ret = false;
#if _DEBUG
		DebugLayerOn();
#endif
		//	ファクトリー初期化
		ret = InitFactory();
		if (ret == false)
		{
			return false;
		}
		//	デバイス初期化
		ret = InitDevice();
		if (ret == false)
		{
			return false;
		}
		//	スワップチェイン初期化
		ret = InitSwapChain(WindowHandle, Width, Height);
		if (ret == false)
		{
			return false;
		}
		//	バックバッファ用ヒープ初期化
		ret = InitBackBufferHeap();
		if (ret == false)
		{
			return false;
		}
		//	深度バッファ用ヒープ初期化
		ret = InitDepthHeap();
		if (ret == false)
		{
			return false;
		}
		//	レンダーターゲット初期化
		ret = InitRenderTarget(Width, Height);
		if (ret == false)
		{
			return false;
		}
		//	フェンスの初期化
		ret = InitFence();
		if (ret == false)
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// 描画開始
	/// </summary>
	/// <param name="Red">0~1.0<f/param>
	/// <param name="Green">0~1.0f</param>
	/// <param name="Blue">0~1.0f</param>
	/// <param name="Alpha">0~1.0f</param>
	void DirectX::BegineRendering(float Red, float Green, float Blue, float Alpha)
	{
		const UINT bufferIndex = SwapChain->GetCurrentBackBufferIndex();

		//	リソースバリアの設定
		ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		ResourceBarrier.Transition.pResource = BackBuffers[bufferIndex];
		ResourceBarrier.Transition.Subresource = 0;
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		CommandList->ResourceBarrier(1, &ResourceBarrier);

		D3D12_CPU_DESCRIPTOR_HANDLE heap = BackBufferHeap->GetCPUDescriptorHandleForHeapStart();
		heap.ptr += static_cast<SIZE_T>(bufferIndex) * static_cast<SIZE_T>(Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		D3D12_CPU_DESCRIPTOR_HANDLE dsvheap = DepthHeap->GetCPUDescriptorHandleForHeapStart();
		//	レンダーターゲット設定
		CommandList->OMSetRenderTargets(1, &heap, true, &dsvheap);

		//	レンダーターゲットクリア
		const float color[4] = { Red,Green,Blue,Alpha };
		CommandList->ClearRenderTargetView(heap, color, 0, nullptr);
		CommandList->ClearDepthStencilView(dsvheap, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	}

	/// <summary>
	/// 画面のフリップ
	/// </summary>
	void DirectX::Flip()
	{
		ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		CommandList->ResourceBarrier(1, &ResourceBarrier);

		//	命令のクローズ
		CommandList->Close();

		//	コマンドリストの実行
		ID3D12CommandList* cmdlists[] = { CommandList };
		CommandQueue->ExecuteCommandLists(1, cmdlists);

		//	画面のスワップ
		SwapChain->Present(1, 0);

		WaitForGPU();
		//D3D12_COMPUTE_PIPELINE_STATE_DESC

		//	キューをクリア
		CommandAllocator->Reset();
		CommandList->Reset(CommandAllocator, nullptr);
	}

	/// <summary>
	/// GPUの処理待ち
	/// </summary>
	void DirectX::WaitForGPU()
	{
		CommandQueue->Signal(Fence, ++FenceValue);

		//	GPUの処理待ち
		const UINT64 CompletedValue = Fence->GetCompletedValue();
		if (CompletedValue < FenceValue)
		{
			Fence->SetEventOnCompletion(FenceValue, WaitForGPUEventHandle);
			WaitForSingleObject(WaitForGPUEventHandle, INFINITE);
		}
	}

	/// <summary>
	/// ビューポートの設定
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void DirectX::SetViewPort(float x, float y, float Width, float Height)
	{
		//	ビューポート設定
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = x;
		viewport.TopLeftY = y;
		viewport.Width = Width;
		viewport.Height = Height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		CommandList->RSSetViewports(1, &viewport);

		//	シザー矩形設定
		D3D12_RECT rect = {};
		rect.top = 0;
		rect.left = 0;
		rect.right = static_cast<LONG>(Width);
		rect.bottom = static_cast<LONG>(Height);
		CommandList->RSSetScissorRects(1, &rect);
	}

	/// <summary>
	/// デバッグレイヤーの起動
	/// </summary>
	void DirectX::DebugLayerOn()
	{
		ID3D12Debug* debugLater = nullptr;
		const HRESULT ret = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLater));
		if (SUCCEEDED(ret))
		{
			debugLater->EnableDebugLayer();
			debugLater->Release();
		}
	}

	/// <summary>
	/// デバイスの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool DirectX::InitDevice()
	{
		bool Success = false;
		for (UINT i = 0;; i++)
		{
			IDXGIAdapter* Adapter = nullptr;
			if (Factory->EnumAdapters(i, &Adapter) == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}
			//	D3D_FEATURE_LEVEL_12_2
			//	D3D_FEATURE_LEVEL_12_1
			//	D3D_FEATURE_LEVEL_12_0 
			//	D3D_FEATURE_LEVEL_11_1
			const D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_12_0;
			//	デバイス初期化
			const HRESULT ret = D3D12CreateDevice(Adapter, level, IID_PPV_ARGS(&Device));
			Adapter->Release();
			if (SUCCEEDED(ret))
			{
				Success = true;
				break;
			}
		}

		//	デバッグ用デバイスの有効化
		if (Device != nullptr)
		{
			Device->QueryInterface(IID_PPV_ARGS(&DebugDevice));
		}

		return Success;
	}

	/// <summary>
	/// ファクトリーの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool DirectX::InitFactory()
	{
#if _DEBUG
		const HRESULT ret = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory));
#else
		const HRESULT ret = CreateDXGIFactory1(IID_PPV_ARGS(&Factory));
#endif
		if (FAILED(ret))
		{
			return false;
		}
		/*
		* Factoryが作成出来ればPCが認識しているGPUデバイスを取得できる
		*/
		return true;
	}

	/// <summary>
	/// スワップチェインの初期化
	/// </summary>
	/// <param name="WindowHandle"></param>
	/// <param name="Width"></param>
	/// <param name="Height"></param>
	/// <returns>true:成功</returns>
	bool DirectX::InitSwapChain(HWND WindowHandle, UINT Width, UINT Height)
	{
		//	アロケーターの初期化
		HRESULT ret = Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator));
		if (FAILED(ret))
		{
			return false;
		}

		//	コマンドリストの初期化
		ret = Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&CommandList));
		if (FAILED(ret))
		{
			return false;
		}

		//	コマンドキュー初期化
		D3D12_COMMAND_QUEUE_DESC cmdQueue = {};
		cmdQueue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueue.NodeMask = 0;
		cmdQueue.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQueue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ret = Device->CreateCommandQueue(&cmdQueue, IID_PPV_ARGS(&CommandQueue));
		if (FAILED(ret))
		{
			return false;
		}

		//	スワップチェイン初期化
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Width = Width;
		scDesc.Height = Height;
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.Stereo = false;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		scDesc.BufferCount = 2;
		scDesc.Scaling = DXGI_SCALING_STRETCH;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//	生成	
		ret = Factory->CreateSwapChainForHwnd(CommandQueue, WindowHandle, &scDesc, nullptr, nullptr, (IDXGISwapChain1**)&SwapChain);
		if (FAILED(ret))
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// バックバッファ用ディスクリプタヒープの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool DirectX::InitBackBufferHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC dchDesc = {};
		//	レンダーターゲットビュー(RTV)
		dchDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		dchDesc.NodeMask = 0;
		dchDesc.NumDescriptors = NumBackBuffer;
		dchDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		//	ディスクリプタヒープ生成
		const HRESULT ret = Device->CreateDescriptorHeap(&dchDesc, IID_PPV_ARGS(&BackBufferHeap));
		if (FAILED(ret))
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// 深度バッファ用ディスクリプタヒープの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool DirectX::InitDepthHeap()
	{
		//	深度用ディスクリプターヒープ
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NodeMask = 0;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		const HRESULT ret = Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DepthHeap));
		if (FAILED(ret))
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// レンダーターゲットの初期化
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns>true:成功</returns>
	bool DirectX::InitRenderTarget(UINT Width, UINT Height)
	{
		HRESULT ret = 0;
		//	レンダーターゲット設定
		D3D12_CPU_DESCRIPTOR_HANDLE handle = BackBufferHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT index = 0; index < 2; index++)
		{
			//	バックバッファー取得
			ret = SwapChain->GetBuffer(index, IID_PPV_ARGS(&BackBuffers[index]));
			if (FAILED(ret))
			{
				return false;
			}

			Device->CreateRenderTargetView(BackBuffers[index], nullptr, handle);

			handle.ptr += Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		}

		//	深度バッファ作成
		D3D12_RESOURCE_DESC depthResource = {};
		depthResource.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthResource.Width = (UINT64)Width;
		depthResource.Height = Height;
		depthResource.DepthOrArraySize = 1;
		depthResource.Format = DXGI_FORMAT_D32_FLOAT;
		depthResource.SampleDesc.Count = 1;
		depthResource.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		//	深度値用ヒープ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		//	クリアバリュー
		D3D12_CLEAR_VALUE clearvalue = {};
		clearvalue.DepthStencil.Depth = 1.0f;
		clearvalue.Format = DXGI_FORMAT_D32_FLOAT;

		ret = Device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &depthResource,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearvalue, IID_PPV_ARGS(&DepthBuffer));
		if (FAILED(ret))
		{
			return false;
		}

		//	深度ビュー
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		Device->CreateDepthStencilView(DepthBuffer, &dsvDesc, DepthHeap->GetCPUDescriptorHandleForHeapStart());

		return true;
	}

	/// <summary>
	/// フェンスの初期化
	/// </summary>
	/// <returns>true:成功</returns>
	bool DirectX::InitFence()
	{
		const HRESULT ret = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
		if (FAILED(ret))
		{
			return false;
		}

		FenceValue = 1;
		//	イベントハンドルの作成
		WaitForGPUEventHandle = CreateEvent(nullptr, false, false, nullptr);
		if (WaitForGPUEventHandle == nullptr)
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// フレームインデックスの取得
	/// </summary>
	/// <returns></returns>
	int DirectX::GetFrameIndex()const
	{
		const int BackBufferIndex = static_cast<int>(SwapChain->GetCurrentBackBufferIndex());
		return BackBufferIndex;
	}
}