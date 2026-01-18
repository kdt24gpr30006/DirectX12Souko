#include "Core.h"
#include "Graphics/DirectX/DirectX.h"
#include "Graphics/GraphicsDescriptorHeap/GraphicsDescriptorHeapManager.h"
#include "Graphics/Renderer/Renderer.h"
#include "System/Window/Window.h"
#include "System/Debug/Debug.h"
#include "System/Input/Input.h"

#include <Plugin/ImGui/imgui.h>
#include <Plugin/ImGui/imgui_impl_dx12.h>
#include <Plugin/ImGui/imgui_impl_win32.h>


//	メモリリークチェック用
#if _DEBUG
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>
#endif

namespace Core
{
	/// <summary>
	/// フレームワークの初期化
	/// </summary>
	/// <returns></returns>
	bool Init()
	{
#if _DEBUG
		//	メモリ解放漏れチェック用
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//	デバッグ機能
		Debug::Init();
#endif

		//	ウィンドウ初期化
		Window::Create();
		Window* WindowInstance = Window::GetInstance();
		WindowInstance->Init("DirectX12", 0, 0);
		//	DirectX初期化
		Graphics::DirectX::Create();
		Graphics::DirectX* DirectXInstance = Graphics::DirectX::GetInstance();
		DirectXInstance->Init(
			WindowInstance->GetWindowHandle(),
			static_cast<UINT>(Window::Width),
			static_cast<UINT>(Window::Height));
		//	描画用ヒープの初期化
		GraphicsDescriptorHeapManager::Create();
		GraphicsDescriptorHeapManager* GDHManager = GraphicsDescriptorHeapManager::GetInstance();
		GDHManager->Init();

		//	描画用クラスの初期化
		Graphics::Renderer::Create();
		//	入力系クラスの初期化
		System::Input::Create();

		/*
		* Imguiの初期化
		* サンプルから流用
		*/
		{
			ImGuiContext* context = ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;

			ImGui_ImplWin32_Init(WindowInstance->GetWindowHandle());

			ImGui_ImplDX12_InitInfo init_info = {};
			init_info.Device = DirectXInstance->GetDevice();
			init_info.CommandQueue = DirectXInstance->GetCommandQueue();
			init_info.NumFramesInFlight = 2;// APP_NUM_FRAMES_IN_FLIGHT;
			init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
			//干渉されないようにヒープの1ブロックを占有する
			const DescriptorHeapInfo info = GDHManager->Issuance(1);
			init_info.SrvDescriptorHeap = GDHManager->GetGraphicsHeap();
			init_info.LegacySingleSrvCpuDescriptor = GDHManager->GetCpuHandle(info);
			init_info.LegacySingleSrvGpuDescriptor = GDHManager->GetGpuHandle(info);
			ImGui_ImplDX12_Init(&init_info);
		}

		return true;
	}
	/// <summary>
	/// フレームワークの解放
	/// </summary>
	void Release()
	{
		//ImGuiの解放
		{
			ImGui_ImplDX12_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}
		System::Input::Release();
		Graphics::Renderer::Release();
		GraphicsDescriptorHeapManager::Release();
		Graphics::DirectX::Release();
		Window::Release();
#if _DEBUG
		Debug::Release();
#endif
	}

	/// <summary>
	/// 新規フレームの開始
	/// </summary>
	void NewFrame()
	{
		System::Input::GetInstance()->Update();
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	/// <summary>
	/// レンダリングの開始
	/// </summary>
	void BegineRendering()
	{
		Graphics::DirectX* DirectXInstance = Graphics::DirectX::GetInstance();
		DirectXInstance->BegineRendering(0.0f, 0.0f, 0.0f, 1.0f);
		DirectXInstance->SetViewPort(0, 0, Window::Width, Window::Height);
		Graphics::Renderer::GetInstance()->NewRenderingFrame();
	}

	/// <summary>
	/// フレームの終了
	/// </summary>
	void EndFrame()
	{
		Graphics::DirectX* DirectXInstance = Graphics::DirectX::GetInstance();
		{
			//ImGuiの描画を行う
			ImGui::Render();

			ID3D12GraphicsCommandList* commandList = DirectXInstance->GetCommandList();
			GraphicsDescriptorHeapManager* GDHManager = GraphicsDescriptorHeapManager::GetInstance();
			ID3D12DescriptorHeap* heap = GDHManager->GetGraphicsHeap();
			commandList->SetDescriptorHeaps(1, &heap);
			ImGui_ImplDX12_RenderDrawData(
				ImGui::GetDrawData(),
				commandList);
		}

		DirectXInstance->Flip();
	}
}