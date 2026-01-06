#include "Entity.h"
#include "Math/Vector3/Vector3.h"
#include "System/Collider/AABB/AABBCollider.h"

Entity::Entity()
{
    SyncCollider();
}

void Entity::Update(float)
{
}

void Entity::Draw()
{
}

const Math::Vector3& Entity::GetPosition() const
{
    return position;
}

void Entity::SetPosition(const Math::Vector3& p)
{
    position = p;
    SyncCollider();
}

void Entity::Move(const Math::Vector3& delta)
{
    position += delta;
    SyncCollider();
}

AABBCollider& Entity::GetCollider()
{
    return collider;
}

const AABBCollider& Entity::GetCollider() const
{
    return collider;
}

bool Entity::IsPushable() const
{
    return false;
}

Math::Vector3 Entity::GetHalfSize() const
{
    // デフォルトは小さな立方体
    return { 0.5f, 0.5f, 0.5f };
}

void Entity::SyncCollider()
{
    collider.SetCenter(position);
    collider.SetVolume(GetHalfSize() * 2.0f);
}
