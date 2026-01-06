#pragma once

#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"
#include "Math/Quaternion/Quaternion.h"

class Stage;
class Block;
class FbxMesh;
class StateMachine;

class Player
{
public:
    static constexpr float MoveSpeed = 10.0f;

    Player();
    ~Player();

    bool Initialize(Stage* stage);
    void Update(float dt);
    void Draw();
    void Release();

    void SetPosition(const Math::Vector3& p) { position = p; }
    const Math::Vector3& GetPosition() const { return position; }

    Stage* GetStage() const { return stage; }

    bool CanPush(Block*& outBlock, Int2& outDir) const;
    void PlayAnimation(const char* name, float dt, bool loop);

    const Math::Vector3& GetForward() const;

private:

    Stage* stage = nullptr;

    Math::Vector3 position{};

    FbxMesh* model = nullptr;

    StateMachine* stateMachine = nullptr;

    Math::Quaternion Rotation;
    mutable Math::Vector3 ForwardCache;
};