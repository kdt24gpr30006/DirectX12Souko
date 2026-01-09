#include "Player.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "../Block/Block.h"
#include "../../Stage/Stage.h"
#include "StateMachine/StateMachine.h"
#include "State/Idle/StateIdle.h"
#include "Math/Int2/Int2.h"
#include "Math/Vector3/Vector3.h"
#include <cmath>
#include <Math/Quaternion/Quaternion.h>
#include "../Entity.h"

Player::Player()
{
}

Player::~Player()
{
    Release();
}

bool Player::Initialize(Stage* inStage)
{
    stage = inStage;

    // 初期グリッド座標
    const Int2 startGrid{ 0, 0 };

    // グリッド中央のワールド座標へ変換
    position = stage->GridToWorld(startGrid);

    // モデル生成
    model = new FbxMesh();
    model->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin");
    model->LoadAnimation("Idle", "Assets/Mannequin/Animation/MM_Idle.FBX.anm");
    model->LoadAnimation("Run", "Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");
    model->LoadAnimation("Push", "Assets/Mannequin/Animation/MM_Push.FBX.anm");

    model->SetScale({ 0.05f,0.05f,0.05f });
    model->SetPosition(position);

    // UEから持ってきたモデルはY軸を90度回転
    constexpr float DEG_TO_RAD = 3.1415926535f / 180.0f;
    Math::Quaternion y90 =
        Math::Quaternion::AngleAxis(270.0f * DEG_TO_RAD, Math::Vector3{ 1.0f, 0.0f, 0.0f });

    SetRotation(y90);

    // ステートマシン
    stateMachine = new StateMachine();
    stateMachine->Initialize(this, new StateIdle());

    return true;
}

void Player::Release()
{
    if (stateMachine)
    {
        delete stateMachine;
        stateMachine = nullptr;
    }

    Entity::Release();
}

void Player::Update(float dt)
{
    stateMachine->Update(this, dt);
}

void Player::PlayAnimation(const char* name, float dt, bool loop)
{
    model->Animate(name, dt, loop);
}

const Math::Vector3& Player::GetForward() const
{
    static const Math::Vector3 baseForward{ 0.0f, 0.0f, 1.0f };

    ForwardCache = Math::Quaternion::Rotate(baseForward, rotation);
    ForwardCache.Normalize();

    return ForwardCache;
}

Int2 Player::GetGridPos() const
{
    return stage->WorldToGrid(GetPosition());
}

bool Player::CanPush(Block*& outBlock, Int2& outDir) const
{
    const Math::Vector3 pos = GetPosition();

    for (auto& blockPtr : stage->GetBlocks())
    {
        Block* block = blockPtr.get(); 

        const Math::Vector3 toBlock = block->GetPosition() - pos;
        const float dist = std::sqrt(toBlock.x * toBlock.x + toBlock.z * toBlock.z);

        // 押せる距離か
        if (dist > 1.1f)
            continue;

        // 押す方向決定
        if (std::fabs(toBlock.x) > std::fabs(toBlock.z))
        {
            outDir = { (toBlock.x > 0) ? 1 : -1, 0 };
        }
        else
        {
            outDir = { 0, (toBlock.z > 0) ? 1 : -1 };
        }

        outBlock = block; 
        return true;
    }
    return false;
}