#include "StateJump.h"
#include "../../Player.h"
#include "System/Input/Input.h"
#include "System/Camera/Camera.h"
#include "Math/Math.h"
#include "Math/Quaternion/Quaternion.h"
#include "Math/Vector2/Vector2.h"
#include "../Idle/StateIdle.h"
#include "../Run/StateRun.h"

StateJump::StateJump()
	: Velocity()
	, GravityAccel()
{
}

StateJump::~StateJump()
{
}

bool StateJump::Initialize(Player* player)
{
	//GravityAccel.y = Player::JumpPower * 0.05f;
	GravityAccel.y = 2.0f;
	player->SetAir();
    return true;
}

void StateJump::Release()
{
}

void StateJump::Process(Player* player, const float deltaTime)
{
	System::Input* input = System::Input::GetInstance();

	GravityAccel.y -= Player::Gravity * deltaTime;
	if (GravityAccel.y <= -Player::Gravity)
	{
		GravityAccel.y = -Player::Gravity;
	}

	Velocity += GravityAccel * deltaTime;

	Math::Vector3 Position = player->GetPosition();
	Position += Velocity;

#if false
	Angle = atan2f(Velocity.x, Velocity.z);
	const Math::Quaternion rot = Math::Quaternion::AngleAxis(-90.0f * Math::RAD, Math::Vector3::Right);
	const Math::Quaternion yrot = Math::Quaternion::AngleAxis(Angle, Math::Vector3::Up);
	Model->SetRotation(rot * yrot);
#endif

	player->SetPosition(Position);
    player->Animate(EPlayerAnimationState::Jump, deltaTime);
}

bool StateJump::CheckCondition(Player* player)
{
	const bool bIsAir = player->IsAir();
	if (bIsAir == false)
	{
		return true;
	}
    return false;
}

StateBase* StateJump::GetNextState()
{
    return new StateIdle();
}
