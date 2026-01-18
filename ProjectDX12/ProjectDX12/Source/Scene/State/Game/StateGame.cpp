#include "StateGame.h"
#include "Graphics/Sprite/Sprite.h"
#include "../External/Plugin/ImGui/imgui.h"
#include "../FrameWork/System/Camera/Camera.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Math.h"
#include "../Source/Entity/Player/Player.h"
#include "../Source/Stage/Stage.h"
#include "../Source/Application/CameraWork/CameraWork.h"
#include <Math/Vector2/Vector2.h>
#include <Math/Int2/Int2.h>
#include <Windows.h>



StateGame::StateGame()
{
}

StateGame::~StateGame()
{
	Exit();
}

void StateGame::Enter()
{
	sprite = new Sprite();
	sprite->Create("Assets/luffy.dds");
	sprite->SetSize(Math::Vector2(512.0f, 512.0f));

	// Stage生成と初期化
	stage = new Stage();
	stage->Initialize();

	// Player生成
	player = new Player();
	player->Initialize(stage);

	// カメラ作成
	camera = new Camera();
	camera->Create();

	// カメラワーク作成
	cameraWork = new CameraWork();
	cameraWork->Initialize(camera);
	cameraWork->SetTarget(player);
}

void StateGame::Update(float dt)
{
	player->Update(dt);

	System::Input* input = System::Input::GetInstance();
	if (input->Keyboard().IsPress(VK_RIGHT))
		cameraWork->AddYaw(+60.0f * Math::RAD * dt);
	if (input->Keyboard().IsPress(VK_LEFT))
		cameraWork->AddYaw(-60.0f * Math::RAD * dt);

	cameraWork->Update(dt);

	stage->Update(dt);

	// 爆発チェック
	if (stage->HasExplosion())
	{
		// TODO: エフェクトやリザルト処理
	}

	if (ImGui::Begin("Debug Grid"))
	{
		// Player
		Int2 p = player->GetGridPos();
		ImGui::Text("Player Grid : (%d, %d)", p.x, p.y);

		// Blocks
		int i = 0;
		for (auto& block : stage->GetBlocks())
		{
			Int2 b = block->GetGridPos();
			ImGui::Text("Block %d Grid : (%d, %d)", i, b.x, b.y);
			++i;
		}

		ImGui::Text("IsExplosion : %d", stage->HasExplosion());
	}
	ImGui::End();
}

void StateGame::Render(float dt)
{
	player->Draw();
	sprite->RenderTexture();
	stage->Render();
}

void StateGame::Exit()
{
	if (sprite)
	{
		delete sprite;
		sprite = nullptr;
	}

	if (player)
	{
		delete player;
		player = nullptr;
	}

	if (stage)
	{
		stage->Release();
		delete stage;
		stage = nullptr;
	}

	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (cameraWork)
	{
		delete cameraWork;
		cameraWork = nullptr;
	}
}
