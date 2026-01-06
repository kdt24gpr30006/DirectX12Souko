#include "Core.h"
#include "System/Window/Window.h"
#include "Application/TestScene.h"
#include <Windows.h>
#include <sal.h>


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	Core::Initialize(); 
	Window* WindowInstance = Window::GetInstance();

	TestScene* Test = new TestScene();
	Test->Initialize();

	while (WindowInstance->IsQuitMessage() == false)
	{
		if (WindowInstance->IsUpdateMessage() == false)
		{
			
			Core::NewFrame();
			/*
			* メイン処理
			* ゲームの更新や描画を記述し実行させる
			*/
			Test->Update();

			Core::BegineRendering();
			
			Test->Render();
			
			Core::EndFrame();
		}
	}
	Test->Release();
	delete Test;

	Core::Release();

	return 0;
}
