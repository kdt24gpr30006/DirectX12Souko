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

    // モデル生成
    model = new FbxMesh();
    model->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin");
    model->LoadAnimation("Idle", "Assets/Mannequin/Animation/MM_Idle.FBX.anm");
    model->LoadAnimation("Run", "Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");
    model->LoadAnimation("Push", "Assets/Mannequin/Animation/MM_Push.FBX.anm");

    model->SetScale({ 0.05f,0.05f,0.05f });
    model->SetPosition(position);
    // ★ Y軸を90度回転
    constexpr float DEG_TO_RAD = 3.1415926535f / 180.0f;
    Math::Quaternion y90 =
        Math::Quaternion::AngleAxis(270.0f * DEG_TO_RAD, Math::Vector3{ 1.0f, 0.0f, 0.0f });

    model->SetRotation(y90);

    // ステートマシン
    stateMachine = new StateMachine();
    stateMachine->Initialize(this, new StateIdle());

    return true;
}

void Player::Release()
{
    if (model)
    {
        model->Release();
        delete model;
        model = nullptr;
    }

    if (stateMachine)
    {
        delete stateMachine;
        stateMachine = nullptr;
    }
}

void Player::Update(float dt)
{
    stateMachine->Update(this, dt);
    model->SetPosition(position);
}

void Player::Draw()
{
    model->Render();
}

void Player::PlayAnimation(const char* name, float dt, bool loop)
{
    model->Animate(name, dt, loop);
}

const Math::Vector3& Player::GetForward() const
{
    static const Math::Vector3 baseForward = { 0.0f, 0.0f, 1.0f };

    ForwardCache = Math::Quaternion::Rotate(baseForward, Rotation);
    ForwardCache.Normalize();

    return ForwardCache;
}

bool Player::CanPush(Block*& outBlock, Int2& outDir) const
{
    // すべてのブロックをチェック
    for (auto& block : stage->GetBlocks())
    {
        // 押せる距離にあるブロックか確認
        const Math::Vector3 toBlock = block.GetPosition() - GetPosition();
        const float dist = std::sqrt(toBlock.x * toBlock.x + toBlock.z * toBlock.z);

        // 押す方向決定（近い面）
        if (std::fabs(toBlock.x) > std::fabs(toBlock.z))
        {
            outDir = { (toBlock.x > 0) ? 1 : -1, 0 };
        }
        else
        {
            outDir = { 0, (toBlock.z > 0) ? 1 : -1 };
        }

        // ブロックを移動
        outBlock = const_cast<Block*>(&block);
        return true;
    }
    return false;
}