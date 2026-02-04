#include "Core.h"
#include "System/Window/Window.h"
#include "Scene/StateMachine/SceneStateMachine.h"
#include "Scene/State/Title/StateTitle.h"
#include "Application/Timer/Timer.h"
#include <Windows.h>
#include <sal.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Core::Init();
	Window* WindowInstance = Window::GetInstance();

	// 高精度タイマー
	GameTimer timer;

	// ゲームのステートマシン
	SceneStateMachine* stateMachine = new SceneStateMachine();
	stateMachine->Init(new StateTitle());

	while (WindowInstance->IsQuitMessage() == false)
	{
		if (WindowInstance->IsUpdateMessage() == false)
		{
			// 実際の経過時間を計測
			float dt = timer.Tick();

			Core::NewFrame();
			/*
			* メイン処理
			* ゲームの更新や描画を記述し実行させる
			*/
			stateMachine->Update(dt);

			Core::BegineRendering();

			stateMachine->Draw(dt);

			Core::EndFrame();
		}
	}

	// 終了処理
	stateMachine->Exit();
	delete stateMachine;

	Core::Release();

	return 0;
}
