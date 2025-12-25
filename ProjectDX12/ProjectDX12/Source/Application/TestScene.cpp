#include "TestScene.h"
#include "System/Camera/Camera.h"
#include "Graphics/Sprite/Sprite.h"
#include "System/Window/Window.h"
#include "System/Input/Input.h"
#include "Math/Math.h"
#include "Field/Field.h"
#include "Player/Player.h"
#include "CameraWork/CameraWork.h"

#include <../External/Plugin/ImGui/imgui.h>
#include "System/Input/Input.h"
#include <winrt/Windows.Gaming.Input.h>

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
	ImGui::Begin("Input Debug");

	auto* input = System::Input::GetInstance();

	// ------------------
	// Keyboard
	// ------------------
	ImGui::Text("Keyboard");
	ImGui::Text("W: %d", input->Keyboard().IsPress('W'));
	ImGui::Text("A: %d", input->Keyboard().IsPress('A'));
	ImGui::Text("S: %d", input->Keyboard().IsPress('S'));
	ImGui::Text("D: %d", input->Keyboard().IsPress('D'));
	ImGui::Text("SPACE: %d", input->Keyboard().IsPress(VK_SPACE));

	ImGui::Separator();

	// ------------------
	// Pad
	// ------------------
	auto& padManager = input->Pad();

	ImGui::Text("Pad Count: %d", padManager.Count());

	if (padManager.Count() > 0)
	{
		auto& pad = padManager.GetPad(0);

		ImGui::Text("Connected: %s", pad.IsConnected() ? "YES" : "NO");

		ImGui::Separator();
		ImGui::Text("Buttons");

		using namespace winrt::Windows::Gaming::Input;

		ImGui::Text("A: %d", pad.IsPress(GamepadButtons::A));
		ImGui::Text("B: %d", pad.IsPress(GamepadButtons::B));
		ImGui::Text("X: %d", pad.IsPress(GamepadButtons::X));
		ImGui::Text("Y: %d", pad.IsPress(GamepadButtons::Y));

		ImGui::Separator();
		ImGui::Text("Stick");

		ImGui::Text("Left X:  %.2f", pad.LeftX());
		ImGui::Text("Left Y:  %.2f", pad.LeftY());
		ImGui::Text("Right X: %.2f", pad.RightX());
		ImGui::Text("Right Y: %.2f", pad.RightY());

		ImGui::Separator();
		ImGui::Text("Trigger");

		ImGui::Text("LT: %.2f", pad.LeftTrigger());
		ImGui::Text("RT: %.2f", pad.RightTrigger());
	}

	ImGui::End();


	field->Render();
	player->Render();
	sprite->RenderTexture();


}