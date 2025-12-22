#include "TestScene.h"
#include "System/Camera/Camera.h"
#include "Graphics/Sprite/Sprite.h"
#include "System/Window/Window.h"
#include "System/Input/Input.h"
#include "Math/Math.h"
#include "Field/Field.h"
#include "Player/Player.h"
#include "CameraWork/CameraWork.h"

TestScene::TestScene()
	: sprite(nullptr)
	, field(nullptr)
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
	sprite->SetSize(Math::Vector2(512.0f, 512.0f));

	field = new Field();
	field->Initialize();

	player = new Player();
	player->Initialize();

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
	field->Update();
	player->Update();

	const AABBCollider* playerCollider = player->GetBoxCollider();
#if true
	Math::Vector3 out;
	const bool bHit = field->CheckFloorCollider(playerCollider, out);
	if (bHit == true)
	{
		player->OnHitFloorCollider(out);
	}
#else
	const bool bHit = field->CheckFieldCollider(playerCollider);
	if (bHit == true)
	{
		player->OnHitCollider();
	}
#endif

	const Math::Vector3 target =
		player->GetPosition() + Math::Vector3(0.0f, 5.0f, 0.0f);
	cameraWork->SetTarget(target);
	cameraWork->Update();
}

void TestScene::Render()
{
	field->Render();
	player->Render();
	sprite->RenderTexture();
}