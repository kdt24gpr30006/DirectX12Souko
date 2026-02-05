#include "StateRun.h"
#include "../../Player.h"
#include "../Idle/StateIdle.h"
#include "System/Input/Input.h"
#include "../../StateMachine/CharaStateMachine.h"
#include "../../../../Application/CameraWork/CameraWork.h"
#include "../../../../Stage/Stage.h"
#include "../../../Block/Block.h"
#include <Math/Vector3/Vector3.h>

void StateRun::Init(Player* player)
{
}

void StateRun::Update(Player* player, float dt)
{
    // トップダウン視点では移動不可 - Idleに戻る
    CameraWork* cam = player->GetCameraWork();
    if (cam && cam->GetCameraMode() == CameraMode::TopDown)
    {
        stateMachine->ChangeState(player, new StateIdle());
        return;
    }

    player->PlayAnimation("Run", dt, true);

    // キーボード取得
    auto& kb = System::Input::GetInstance()->Keyboard();

    // 入力を取得（ローカル座標系: W=前、S=後、A=左、D=右）
    float inputForward = 0.0f;
    float inputRight = 0.0f;
    if (kb.IsPress('W')) inputForward += 1.0f;
    if (kb.IsPress('S')) inputForward -= 1.0f;
    if (kb.IsPress('A')) inputRight -= 1.0f;
    if (kb.IsPress('D')) inputRight += 1.0f;

    // 入力がなければIdleに
    if (inputForward == 0.0f && inputRight == 0.0f)
    {
        stateMachine->ChangeState(player, new StateIdle());
        return;
    }

    // カメラ方向を基準にワールド座標の移動方向を計算（FPS視点のみ）
    Math::Vector3 camForward = cam->GetForwardXZ();
    Math::Vector3 camRight = cam->GetRightXZ();

    Math::Vector3 move = camForward * inputForward + camRight * inputRight;
    move.Normalize();

    // プレイヤーの向きを移動方向に合わせる
    player->SetFacingDirection(move);

    // 移動
    player->SetPosition(player->GetPosition() + move * Player::MoveSpeed * dt);

    // コライダー位置を更新
    player->UpdateCollider();

    // 当たり判定
    Stage* stage = player->GetStage();
    if (stage)
    {
        // ブロックとの当たり判定
        const auto& blocks = stage->GetBlocks();
        for (const auto& block : blocks)
        {
            Math::Vector3 pushOut;
            if (player->GetCollider()->CheckHit(block->GetCollider(), pushOut))
            {
                // 押し戻し（最も浅い軸方向に押し出す）
                Math::Vector3 pos = player->GetPosition();
                Math::Vector3 toBlock = block->GetPosition() - pos;

                // 最小のめり込み軸を選択して押し戻す
                if (pushOut.x <= pushOut.y && pushOut.x <= pushOut.z)
                {
                    // X軸方向に押し戻し
                    pos.x += (toBlock.x > 0) ? -pushOut.x : pushOut.x;
                }
                else if (pushOut.z <= pushOut.x && pushOut.z <= pushOut.y)
                {
                    // Z軸方向に押し戻し
                    pos.z += (toBlock.z > 0) ? -pushOut.z : pushOut.z;
                }

                player->SetPosition(pos);
                player->UpdateCollider();
            }
        }

        // 壁との当たり判定
        const auto& walls = stage->GetWallColliders();
        for (const auto& wall : walls)
        {
            Math::Vector3 pushOut;
            if (player->GetCollider()->CheckHit(&wall, pushOut))
            {
                // 押し戻し（最も浅い軸方向に押し出す）
                Math::Vector3 pos = player->GetPosition();

                // 壁の中心位置を取得
                AABBCollider::Range xRange = wall.GetAxisXRange();
                AABBCollider::Range zRange = wall.GetAxisZRange();
                float wallCenterX = (xRange.Min + xRange.Max) * 0.5f;
                float wallCenterZ = (zRange.Min + zRange.Max) * 0.5f;
                Math::Vector3 toWall = Math::Vector3(wallCenterX, 0, wallCenterZ) - pos;

                // 最小のめり込み軸を選択して押し戻す
                if (pushOut.x <= pushOut.y && pushOut.x <= pushOut.z)
                {
                    // X軸方向に押し戻し
                    pos.x += (toWall.x > 0) ? -pushOut.x : pushOut.x;
                }
                else if (pushOut.z <= pushOut.x && pushOut.z <= pushOut.y)
                {
                    // Z軸方向に押し戻し
                    pos.z += (toWall.z > 0) ? -pushOut.z : pushOut.z;
                }

                player->SetPosition(pos);
                player->UpdateCollider();
            }
        }
    }
}
