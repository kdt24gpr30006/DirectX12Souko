#include "Player.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "System/Input/Input.h"
#include "System/Camera/Camera.h"
#include "Math/Math.h"
#include "System/Collider/AABB/AABBCollider.h"
#include "StateMachine/StateMachine.h"
#include "State/Idle/StateIdle.h"

#include <Plugin/ImGui/imgui.h>

Player::Player()
	: Model(nullptr)
	, BoxCollider(nullptr)
	, BoxColliderVolume()
	, stateMachine(nullptr)
	, Accel()
	, GravityAccel()
	, Velocity()
	, Angle()
	, isAir(false)
	, CurrentAnimationState()
{
}

Player::~Player()
{
}

bool Player::Initialize()
{
	Model = new FbxMesh();
#if false
	Model->Create("Assets/ParagonCrunch/Crunch.FBX.bin");
	Model->LoadAnimation("Idle", "Assets/ParagonCrunch/Animation/Idle_Combat.FBX.anm");
	Model->LoadAnimation("Run", "Assets/ParagonCrunch/Animation/Jog_Fwd.FBX.anm");
	Model->LoadAnimation("Jump", "Assets/ParagonCrunch/Animation/Jump_Start.FBX.anm");
	Model->LoadAnimation("FallLoop", "Assets/ParagonCrunch/Animation/Jump_Apex.FBX.anm");
	Model->LoadAnimation("Land", "Assets/ParagonCrunch/Animation/Jump_Land.FBX.anm");
#else
	Model->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin");
	Model->LoadAnimation("Idle","Assets/Mannequin/Animation/MM_Idle.FBX.anm");
	Model->LoadAnimation("Run","Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");
	Model->LoadAnimation("Jump","Assets/Mannequin/Animation/MM_Jump.FBX.anm");
	Model->LoadAnimation("FallLoop","Assets/Mannequin/Animation/MM_Fall_Loop.FBX.anm");
	Model->LoadAnimation("Land","Assets/Mannequin/Animation/MM_Land.FBX.anm");
#endif
	/*
	* TODO:UEからエクスポートしたモデルだけ-90度回転しないといけない
	*/
	Model->SetRotation(Math::Quaternion::AngleAxis(-90.0f * Math::RAD, Math::Vector3::Right));
	const float Scale = 0.05f;
	Model->SetScale(Math::Vector3(Scale, Scale, Scale));

	BoxCollider = new AABBCollider();
	BoxColliderVolume.x = 3.0f;
	BoxColliderVolume.y = 8.0f;
	BoxColliderVolume.z = 3.0f;
	BoxCollider->SetVolume(BoxColliderVolume);

	SetPosition(Math::Vector3(-5.0f, 0.0f, 0.0f));

	//	ステートマシン初期化
	stateMachine = new StateMachine();
	stateMachine->Initialize(this, new StateIdle());

	return true;
}
void Player::Release()
{
	if (Model != nullptr)
	{
		Model->Release();
		delete Model;
		Model = nullptr;
	}
	if (BoxCollider != nullptr)
	{
		delete BoxCollider;
		BoxCollider = nullptr;
	}
	if (stateMachine != nullptr)
	{
		stateMachine->Release();
		delete stateMachine;
		stateMachine = nullptr;
	}
}

void Player::GuiUpdate()
{
	if (ImGui::Begin("Player Parameter"))
	{
		ImGui::InputFloat3("AABBCollider",
			&BoxColliderVolume[0]);

		BoxCollider->SetVolume(BoxColliderVolume);

		ImGui::InputFloat("x",
			&Model->GetPosition().x);
		ImGui::InputFloat("y",
			&Model->GetPosition().y);
		ImGui::InputFloat("z",
			&Model->GetPosition().z);
	}
	ImGui::End();
}

/*
* ステートマシンを使う方
*/
void Player::Update()
{
	OldPosition = GetPosition();
	const float deltaTime = 0.0166666f;
	stateMachine->Process(this, deltaTime);
	GuiUpdate();
}

void Player::Render()
{
	Model->Render();
	BoxCollider->DebugRender();
}

const Math::Vector3& Player::GetPosition()const
{
	return Model->GetPosition();
}

void Player::SetPosition(const Math::Vector3& position)
{
	//	座標の指定とコライダーの中心位置も補正する
	Model->SetPosition(position);
	Math::Vector3 center = position;
	center.y += BoxColliderVolume.y * 0.5f;
	BoxCollider->SetCenter(center);
}

const Math::Quaternion& Player::GetRotation()const
{
	return Model->GetRotation();
}

void Player::SetRotation(const Math::Quaternion& rotation)
{
	Model->SetRotation(rotation);
}

const AABBCollider* Player::GetBoxCollider()const
{
	return BoxCollider;
}

void Player::Animate(EPlayerAnimationState state, float deltaTime)
{
	bool bIsLoop = false;
	switch (state)
	{
	case EPlayerAnimationState::Idle:
		CurrentAnimationState = "Idle";
		bIsLoop = true;
		break;
	case EPlayerAnimationState::Run:
		CurrentAnimationState = "Run";
		bIsLoop = true;
		break;
	case EPlayerAnimationState::Jump:
		CurrentAnimationState = "Jump";
		bIsLoop = false;
		break;
	case EPlayerAnimationState::FallLoop:
		CurrentAnimationState = "FallLoop";
		bIsLoop = true;
		break;
	case EPlayerAnimationState::Land:
		CurrentAnimationState = "Land";
		bIsLoop = false;
		break;
	default:
		break;
	}
	Model->Animate(CurrentAnimationState, deltaTime, bIsLoop);
}

void Player::OnHitFloorCollider(const Math::Vector3& v)
{
	Math::Vector3 Position = GetPosition();
	Position.y += v.y;
	SetPosition(Position);
	GravityAccel = Math::Vector3::Zero;
	isAir = false;
}

void Player::OnHitCollider(const Math::Vector3& v)
{
	Math::Vector3 Position = GetPosition();
	Math::Vector3 move = Position - OldPosition;
	if (isAir == true)
	{
		isAir = false;
	}

	//Position.x += v.x;
	Position.y += v.y;
	//Position.z += v.z;

	Velocity = Math::Vector3::Zero;
	Accel = Math::Vector3::Zero;
	GravityAccel = Math::Vector3::Zero;

	SetPosition(Position);
}