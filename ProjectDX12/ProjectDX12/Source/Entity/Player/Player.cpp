#include "Player.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "../Block/Block.h"
#include "../../Stage/Stage.h"
#include "StateMachine/CharaStateMachine.h"
#include "State/Idle/StateIdle.h"
#include "State/Push/StatePush.h"
#include "Math/Int2/Int2.h"
#include "Math/Vector3/Vector3.h"
#include <Math/Quaternion/Quaternion.h>
#include "../Entity.h"
#include "../FrameWork/System/Input/Input.h"
#include <cmath>

Player::Player()
{
}

Player::~Player()
{
    Release();
}

bool Player::Init(Stage* inStage)
{
    stage = inStage;

    // 初期グリッド座標（壁の内側）
    const Int2 startGrid{ 1, 1 };

    // グリッド中央のワールド座標へ変換して配置
    position = stage->GridToWorld(startGrid);

    // モデル生成と基本設定
    model = new FbxMesh();
    model->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin");
    model->LoadAnimation("Idle", "Assets/Mannequin/Animation/MM_Idle.FBX.anm");
    model->LoadAnimation("Run", "Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");
    model->LoadAnimation("Push", "Assets/Mannequin/Animation/MM_Push.FBX.anm");

    model->SetScale({ 0.05f,0.05f,0.05f });
    model->SetPosition(position);

    SetRotation(x90);

    // コライダー初期化（プレイヤーサイズに合わせる）
    collider.SetVolume(Math::Vector3(5.0f, 10.0f, 5.0f));
    collider.SetCenter(position);

    // プレイヤー用ステートマシン初期化（初期はIdle）
    stateMachine = new CharaStateMachine();
    stateMachine->Init(this, new StateIdle());

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

    model->SetPosition(GetPosition());
    model->SetRotation(GetRotation());
}
void Player::PlayAnimation(const char* name, float dt, bool loop)
{
    model->Animate(name, dt, loop);
}

void Player::SetFacingDirection(const Math::Vector3& dir)
{
    if (dir.x == 0 && dir.z == 0)
        return;

    float yaw = std::atan2(dir.x, dir.z);

    Math::Quaternion rot =
        Math::Quaternion::AngleAxis(yaw, Math::Vector3{ 0.0f, 1.0f, 0.0f });

    SetRotation(x90 * rot);

    // 移動方向からグリッド方向（Int2）を計算
    // 最も大きい成分の方向を使う
    if (std::fabs(dir.x) > std::fabs(dir.z))
    {
        facingDir = { (dir.x > 0) ? 1 : -1, 0 };
    }
    else
    {
        facingDir = { 0, (dir.z > 0) ? 1 : -1 };
    }
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

void Player::UpdateCollider()
{
    collider.SetCenter(position);
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

    // StatePushに遷移（ブロック押し処理はStatePush::Init()で実行される）
    stateMachine->ChangeState(this, new StatePush(block, facingDir));
}
