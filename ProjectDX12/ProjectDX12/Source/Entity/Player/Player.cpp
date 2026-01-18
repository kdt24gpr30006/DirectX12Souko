#include "Player.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "../Block/Block.h"
#include "../../Stage/Stage.h"
#include "StateMachine/StateMachine.h"
#include "State/Idle/StateIdle.h"
#include "Math/Int2/Int2.h"
#include "Math/Vector3/Vector3.h"
#include <Math/Quaternion/Quaternion.h>
#include "../Entity.h"
#include "../FrameWork/System/Input/Input.h"

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

    // 初期グリッド座標（仮）
    const Int2 startGrid{ 0, 0 };

    // グリッド中央のワールド座標へ変換して配置
    position = stage->GridToWorld(startGrid);

    // モデル生成と基本設定
    model = new FbxMesh();
    model->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin");
    model->LoadAnimation("Idle", "Assets/Mannequin/Animation/MM_Idle.FBX.anm");
    model->LoadAnimation("Run", "Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");

    model->SetScale({ 0.05f,0.05f,0.05f });
    model->SetPosition(position);

    // UE由来モデルなので向きを補正（Y軸回転）
    constexpr float DEG_TO_RAD = 3.1415926535f / 180.0f;
    Math::Quaternion y90 =
        Math::Quaternion::AngleAxis(270.0f * DEG_TO_RAD, Math::Vector3{ 1.0f, 0.0f, 0.0f });

    SetRotation(y90);

    // プレイヤー用ステートマシン初期化（初期はIdle）
    stateMachine = new StateMachine();
    stateMachine->Initialize(this, new StateIdle());

    return true;
}

void Player::Release()
{
    // ステートマシン解放
    if (stateMachine)
    {
        delete stateMachine;
        stateMachine = nullptr;
    }

    Entity::Release();
}

void Player::Update(float dt)
{
    // 入力から向きだけ更新（移動とは独立）
    UpdateFacingFromInput();

    // 状態更新（移動・アニメーションなど）
    stateMachine->Update(this, dt);

    // 押す操作はEキー入力でのみ判定
    TryPushBlock();
}

void Player::PlayAnimation(const char* name, float dt, bool loop)
{
    model->Animate(name, dt, loop);
}

const Math::Vector3& Player::GetForward() const
{
    // モデルの回転から前方ベクトルを計算
    static const Math::Vector3 baseForward{ 0.0f, 0.0f, 1.0f };

    ForwardCache = Math::Quaternion::Rotate(baseForward, rotation);
    ForwardCache.Normalize();

    return ForwardCache;
}

Int2 Player::GetGridPos() const
{
    // ワールド座標をグリッド座標に変換
    return stage->WorldToGrid(GetPosition());
}

void Player::UpdateFacingFromInput()
{
    auto& kb = System::Input::GetInstance()->Keyboard();

    Int2 input{ 0,0 };

    // 押されているキーから向きを決定（最後に押された方向が優先）
    if (kb.IsPress('W'))      input = { 0, 1 };
    else if (kb.IsPress('S')) input = { 0,-1 };
    else if (kb.IsPress('A')) input = { -1, 0 };
    else if (kb.IsPress('D')) input = { 1, 0 };

    // 入力があるときのみ向きを更新
    if (input != Int2::Zero)
    {
        facingDir = input;
    }
}

void Player::TryPushBlock()
{
    // Eキーを押した瞬間だけブロックを押せる
    if (System::Input::GetInstance()->Keyboard().IsPush('E') == false)
        return;

    // 向きが未確定なら何もしない
    if (facingDir == Int2::Zero)
        return;

    const Int2 playerGrid = GetGridPos();
    const Int2 targetGrid = playerGrid + facingDir;

    // プレイヤーの正面にあるブロックを取得
    Block* block = stage->GetBlockAt(targetGrid);
    if (!block)
        return;

    // 移動中のブロックは押せない
    if (block->IsMoving())
        return;

    // 押し処理はStage側に委譲
    stage->TryPush(*block, facingDir);
}
