#pragma once
#include <Math/Vector3/Vector3.h>
#include <Math/Quaternion/Quaternion.h>

class FbxMesh;

class Entity
{
protected:
    Math::Vector3 position{ 0,0,0 };
    Math::Quaternion rotation = Math::Quaternion::Identity;
    FbxMesh* model = nullptr;

public:

    virtual ~Entity() = default;

    virtual bool Initialize() { return true; }
    virtual void Update(float) {}
    virtual void Draw();
    virtual void Release();

    void SetPosition(const Math::Vector3& pos);
    void SetRotation(const Math::Quaternion& rot);

    const Math::Vector3& GetPosition() const { return position; }
    const Math::Quaternion& GetRotation() const { return rotation; }

protected:
};