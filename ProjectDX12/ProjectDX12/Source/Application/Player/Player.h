#pragma once

#include "Math/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"
#include <string>
#include <stdint.h>

class FbxMesh;
class AABBCollider;
class StateMachine;

/// <summary>
/// プレイヤーのアニメーションの列挙
/// </summary>
enum class EPlayerAnimationState : uint8_t
{
	Idle,
	Run,
	Jump,
	FallLoop,
	Land,
};

class Player
{
public:
	/// <summary>
	/// 重力加速度
	/// </summary>
	//static constexpr float Gravity = 9.80665f;
	static constexpr float Gravity = 4.0f;

	/// <summary>
	/// 摩擦力
	/// </summary>
	static constexpr float Friction = 0.9f;

	/// <summary>
	/// 加速度
	/// </summary>
	static constexpr float Acceleration = 30.0f;

	/// <summary>
	/// 最高速度
	/// </summary>
	static constexpr float MaxVelocity = 4.0f;

	/// <summary>
	/// ジャンプ時の強さ
	/// </summary>
	static constexpr float JumpPower = 50.0f;
public:
	Player();
	~Player();

	bool Initialize();
	void Release();
	void Update();
	void Render();

	const Math::Vector3& GetPosition()const;
	void SetPosition(const Math::Vector3& position);

	const Math::Quaternion& GetRotation()const;
	void SetRotation(const Math::Quaternion& rotation);

	const AABBCollider* GetBoxCollider()const;

	void OnHitCollider();
	void OnHitFloorCollider(const Math::Vector3& v);

	void Animate(EPlayerAnimationState state, float deltaTime);

	bool IsAir()const { return isAir; }
	void SetAir() { isAir = true; }
private:

	void GuiUpdate();

	void OnHitCollider(const Math::Vector3& v);

	FbxMesh* Model;
	AABBCollider* BoxCollider;
	Math::Vector3 BoxColliderVolume;

	StateMachine* stateMachine;

	Math::Vector3 Accel;
	Math::Vector3 GravityAccel;

	Math::Vector3 Velocity;
	Math::Vector3 OldPosition;

	float Angle;

	bool isAir;

	std::string CurrentAnimationState;
};

