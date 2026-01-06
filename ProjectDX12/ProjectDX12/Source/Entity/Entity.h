#pragma once
#include "Math/Vector3/Vector3.h"
#include "../FrameWork/System/Collider/AABB/AABBCollider.h"

/// <summary>
/// ワールドに存在する全オブジェクトの基底クラス
/// Player / Block / Goal などはすべてこれを継承する
/// </summary>
class Entity
{
public:
    virtual ~Entity() = default;

    // ===== 更新 / 描画 =====
    virtual void Update(float dt);
    virtual void Draw();

    // ===== Transform =====
    const Math::Vector3& GetPosition() const;
    virtual void SetPosition(const Math::Vector3& p);
    virtual void Move(const Math::Vector3& delta);

    // ===== Collider =====
    AABBCollider& GetCollider();
    const AABBCollider& GetCollider() const;

    // ===== 種別判定 =====
    virtual bool IsPushable() const;

protected:
    Entity();

    /// <summary>
    /// 派生クラスでサイズ指定
    /// </summary>
    virtual Math::Vector3 GetHalfSize() const;

    void SyncCollider();

protected:
    Math::Vector3 position{ 0,0,0 };
    AABBCollider collider;
};
