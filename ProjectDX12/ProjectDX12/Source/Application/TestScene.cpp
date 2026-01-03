#include "TestScene.h"
#include "System/Camera/Camera.h"
#include "Graphics/Sprite/Sprite.h"
#include "System/Window/Window.h"
#include "System/Input/Input.h"
#include "Math/Math.h"
#include "Field/Field.h"
#include "../Stage/Stage.h" 
#include "../Entity/Player/Player.h"
#include "CameraWork/CameraWork.h"

TestScene::TestScene()
	: sprite(nullptr)
	, field(nullptr)
	, stage(nullptr)
	, player(nullptr)
	, cameraWork(nullptr)
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

	field = new Field();
	field->Initialize();

	stage = new Stage();
	stage->Initialize();

	player = new Player();
	player->Initialize(stage); 

	cameraWork = new CameraWork();
	cameraWork->Initialize();

	return true;
}

void TestScene::Release()
{
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}

	if (field != nullptr)
	{
		field->Release();
		delete field;
		field = nullptr;
	}

	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}

	if (cameraWork != nullptr)
	{
		cameraWork->Release();
		delete cameraWork;
		cameraWork = nullptr;
	}

}

void TestScene::Update()
{
	const float dt = 1.0f / 60.0f;

	field->Update();
	player->Update(dt);

	// ===== Field との物理衝突 =====
	Math::Vector3 out{};
	const AABBCollider& playerCol = player->GetCollider();

	if (field->CheckFloorCollider(&playerCol, out))
	{
		// Entity にある Move を使用（存在する）
		player->Move(out);
	}

	// ===== カメラ =====
	cameraWork->SetTarget(
		player->GetPosition() + Math::Vector3(0, 5, 0)
	);
	cameraWork->Update();
}

void TestScene::Render()
{
	field->Render();

	// Stage は Block を持っているので Scene 側で描画
	for (const Block& b : stage->GetBlocks())
	{
		const_cast<Block&>(b).Draw();
	}

	player->Draw();
	sprite->RenderTexture();
}