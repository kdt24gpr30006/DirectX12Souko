#include "Core.h"
#include "System/Window/Window.h"
#include "../Source/Scene/State/Game/StateGame.h"
#include <Windows.h>
#include <sal.h>


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Core::Initialize(); 
	Window* WindowInstance = Window::GetInstance();

	// シーンのテスト
	StateGame* Test = new StateGame();
	Test->Enter();

	// 仮のデルタタイム
	const float dt = 1.0f / 60.0f;

	while (WindowInstance->IsQuitMessage() == false)
	{
		if (WindowInstance->IsUpdateMessage() == false)
		{
			
			Core::NewFrame();
			/*
			* メイン処理
			* ゲームの更新や描画を記述し実行させる
			*/
			Test->Update(dt);

			Core::BegineRendering();
			
			Test->Render(dt);
			
			Core::EndFrame();
		}
	}
	Test->Exit();
	delete Test;

	Core::Release();

	return 0;
}
