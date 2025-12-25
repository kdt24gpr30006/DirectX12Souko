#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "System/Input/Input.h"
#include <Windows.h>
#include "Math/Math.h"

CameraWork::CameraWork()
	: camera(nullptr)
	, Target()
	, Angle()
{
}
CameraWork::~CameraWork()
{
}

bool CameraWork::Initialize()
{
	camera = new Camera();
	camera->Create();

	return true;
}

void CameraWork::Release()
{
	if (camera != nullptr)
	{
		delete camera;
		camera = nullptr;
	}
}

void CameraWork::Update()
{
	const float distance = 8.0f;
	System::Input* input = System::Input::GetInstance();
	if (input->Keyboard().IsPress(VK_RIGHT))
	{
		Angle += Math::RAD * 60.0f * 0.016f;
	}
	if (input->Keyboard().IsPress(VK_LEFT))
	{
		Angle -= Math::RAD * 60.0f * 0.016f;
	}
	Math::Vector3 Position = {};
	Position.x = distance * sinf(Angle) + Target.x;
	Position.y = 1.0f + Target.y;
	Position.z = distance * cosf(Angle) + Target.z;

	camera->Update(
		Position,
		Target,
		Math::Vector3::Up);
}

void CameraWork::Render()
{

}