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

    virtual bool Init() { return true; }
    virtual void Update(float) {}
    virtual void Draw();
    virtual void Release();

    /// <summary>
    /// 座標のセッター
    /// </summary>
    /// <param name="pos"></param>
    void SetPosition(const Math::Vector3& pos);

    /// <summary>
    /// 回転のセッター
    /// </summary>
    /// <param name="rot"></param>
    void SetRotation(const Math::Quaternion& rot);

    const Math::Vector3& GetPosition() const { return position; }
    const Math::Quaternion& GetRotation() const { return rotation; }

protected:
};