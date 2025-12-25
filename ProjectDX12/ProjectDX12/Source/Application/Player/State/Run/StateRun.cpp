#include "StateRun.h"
#include "../../Player.h"
#include "System/Input/Input.h"
#include "System/Camera/Camera.h"
#include "Math/Math.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Vector2/Vector2.h"
#include "../Idle/StateIdle.h"
#include "../Jump/StateJump.h"


StateRun::StateRun()
	: Velocity()
	, Accel()
	, Angle(0.0f)
	, TransitionFlag(0)
{
}
StateRun::~StateRun()
{
}

bool StateRun::Initialize(Player* player)
{
	return true;
}

void StateRun::Release()
{
}

void StateRun::Process(Player* player, const float deltaTime)
{
	Camera* mainCamera = Camera::Main;
	Math::Vector3 forward = mainCamera->GetForward();
	forward.y = 0;
	forward.Normalize();

	Math::Vector3 right = mainCamera->GetRight();
	right.y = 0;
	right.Normalize();

	//地上での挙動
	Velocity *= Player::Friction;

	//微小な値を0に収める
	//if (Velocity.SqrLength() < FLT_EPSILON)
	if (Velocity.SqrLength() < 0.00001f)
	{
		Velocity.x = 0.0f;
		Velocity.y = 0.0f;
		Velocity.z = 0.0f;
	}

	Math::Vector3 v = {};
	const System::Input* input = System::Input::GetInstance();
	if (input->Keyboard().IsPress('D'))
	{
		v += right;
	}
	if (input->Keyboard().IsPress('A'))
	{
		v -= right;
	}

	if (input->Keyboard().IsPress('W'))
	{
		v += forward;
	}
	if (input->Keyboard().IsPress('S'))
	{
		v -= forward;
	}

	v.Normalize();
	Accel = v * (Player::Acceleration * deltaTime);
	if (Accel.SqrLength() >= Player::Acceleration * Player::Acceleration)
	{
		Accel.Normalize();
		Accel *= Player::Acceleration;
	}

	Velocity += Accel * deltaTime;
	//横移動の最高速の制限
	Math::Vector2 velo = { Velocity.x, Velocity.z };

	if (velo.SqrLength() >= Player::MaxVelocity * Player::MaxVelocity)
	{
		velo.Normalize();
		velo.x *= Player::MaxVelocity;
		velo.y *= Player::MaxVelocity;
		Velocity.x = velo.x;
		Velocity.z = velo.y;
	}

	Math::Vector3 Position = player->GetPosition();
	Position += Velocity;

	Angle = atan2f(Velocity.x, Velocity.z);

	const Math::Quaternion rot = Math::Quaternion::AngleAxis(-90.0f * Math::RAD, Math::Vector3::Right);
	const Math::Quaternion yrot = Math::Quaternion::AngleAxis(Angle, Math::Vector3::Up);
	player->SetRotation(rot * yrot);
	player->SetPosition(Position);

	player->Animate(EPlayerAnimationState::Run, deltaTime);
}

bool StateRun::CheckCondition(Player* player)
{
	const System::Input* input = System::Input::GetInstance();
	if (input->Keyboard().IsPush(VK_SPACE))
	{
		TransitionFlag |= JUMP_KEY;
		return true;
	}
	if (Velocity.x == 0 &&
		Velocity.y == 0 &&
		Velocity.z == 0)
	{
		TransitionFlag |= TO_IDLE;
		return true;
	}
	return false;
}

StateBase* StateRun::GetNextState()
{
	if (TransitionFlag & JUMP_KEY)
	{
		return new StateJump();
	}
	if (TransitionFlag & TO_IDLE)
	{
		return new StateIdle();
	}
	return nullptr;
}
