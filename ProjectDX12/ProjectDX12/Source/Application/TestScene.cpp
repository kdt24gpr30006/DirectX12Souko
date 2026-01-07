#include "TestScene.h"
#include "Graphics/Sprite/Sprite.h"
#include "../Entity/Player/Player.h"
#include "../Stage/Stage.h"
#include "../../FrameWork/System/Camera/Camera.h"
#include "../Source/Application/CameraWork/CameraWork.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Math.h"
#include <Math/Vector2/Vector2.h>
#include <Windows.h>

#include "../External/Plugin/ImGui/imgui.h"


TestScene::TestScene()
	: sprite(nullptr)
	, player(nullptr)
	, stage(nullptr)
{
}
TestScene::~TestScene()
{
	Release();
}

bool TestScene::Initialize()
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

	return true;
}

void TestScene::Release()
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

void TestScene::Update()
{
	const float dt = 1.0f / 60.0f;
	player->Update(dt);

	System::Input* input = System::Input::GetInstance();
	if (input->Keyboard().IsPress(VK_RIGHT))
		cameraWork->AddYaw(+60.0f * Math::RAD * dt);
	if (input->Keyboard().IsPress(VK_LEFT))
		cameraWork->AddYaw(-60.0f * Math::RAD * dt);

	cameraWork->Update(dt);

	stage->Update(dt);

	// 爆発チェック（デバッグ用）
	if (stage->HasExplosion())
	{
		// TODO: エフェクトやリザルト処理
	}

	if (ImGui::Begin("Debug Grid"))
	{
		// Player
		{
			Int2 p = player->GetGridPos();
			ImGui::Text("Player Grid : (%d, %d)", p.x, p.y);
		}

		// Blocks
		int i = 0;
		for (auto& block : stage->GetBlocks())
		{
			Int2 b = block->GetGridPos();
			ImGui::Text("Block %d Grid : (%d, %d)", i, b.x, b.y);
			++i;
		}
	}
	ImGui::End();

}

void TestScene::Render()
{
	player->Draw();
	sprite->RenderTexture();
	stage->Render();
}
