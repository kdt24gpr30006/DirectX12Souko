#include "Core.h"
#include "System/Window/Window.h"
#include "Scene/StateMachine/SceneStateMachine.h"
#include "Scene/State/Game/StateGame.h"
#include <Windows.h>
#include <sal.h>


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Core::Initialize(); 
	Window* WindowInstance = Window::GetInstance();

	// 仮のデルタタイム
	const float dt = 1.0f / 60.0f;

	// ゲームのステートマシン
	SceneStateMachine* stateMachine = new SceneStateMachine();
	stateMachine->Initialize(new StateGame());

	while (WindowInstance->IsQuitMessage() == false)
	{
		if (WindowInstance->IsUpdateMessage() == false)
		{
			
			Core::NewFrame();
			/*
			* メイン処理
			* ゲームの更新や描画を記述し実行させる
			*/
			stateMachine->Update(dt);

			Core::BegineRendering();
			
			stateMachine->Render(dt);
			
			Core::EndFrame();
		}
	}

	// 終了処理
	stateMachine->Exit();
	delete stateMachine;

	Core::Release();

	return 0;
}
