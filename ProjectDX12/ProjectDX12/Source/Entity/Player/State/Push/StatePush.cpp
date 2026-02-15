#include "StatePush.h"
#include "../../Player.h"
#include "../Idle/StateIdle.h"
#include "../../../../Stage/Stage.h"
#include "../../StateMachine/CharaStateMachine.h"
#include "../../../../Stage/GameTypes.h"
#include "Math/Int2/Int2.h"
#include "../../../Block/Block.h"
#include <algorithm>
#include <cmath>

StatePush::StatePush(Block* b, const Int2& d)
    : block(b), dir(d)
{
}

void StatePush::Init(Player* player)
{
    Stage* stage = player->GetStage();

    // プレイヤーの開始位置と目標位置を計算
    playerStartPos = player->GetPosition();

    // プレイヤーも1マス分前に進む
    Int2 playerGridPos = stage->WorldToGrid(playerStartPos);
    Int2 playerTargetGrid = playerGridPos + dir;
    playerTargetPos = stage->GridToWorld(playerTargetGrid);

    // ブロックを押す処理を実行
    MoveResult result = stage->TryPush(*block, dir);
    pushSucceeded = (result != MoveResult::Blocked);

    if (!pushSucceeded)
    {
        // 押せなかった場合は移動しない
        playerTargetPos = playerStartPos;
    }

    // Push方向にプレイヤーの向きを合わせる
    player->SetFacingDirection(Math::Vector3{
        static_cast<float>(dir.x), 0.0f, static_cast<float>(dir.y)
    });
    player->SetPushing(true);
}

void StatePush::Update(Player* player, float dt)
{
    // Pushアニメーション再生
    player->PlayAnimation("Push", dt, false);

    if (pushSucceeded)
    {
        // プレイヤーをブロックと一緒に移動
        Math::Vector3 currentPos = player->GetPosition();
        Math::Vector3 toTarget = playerTargetPos - currentPos;
        float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);

        if (dist > 0.01f)
        {
            // 移動方向
            Math::Vector3 moveDir = toTarget;
            moveDir.Normalize();

            // 移動量（目標を超えないように）
            float moveAmount = std::min(PUSH_MOVE_SPEED * dt, dist);
            player->SetPosition(currentPos + moveDir * moveAmount);
        }
        else
        {
            // 目標に到達
            player->SetPosition(playerTargetPos);
        }
    }

    // ブロックの移動が完了したらIdleに戻る
    if (!block->IsMoving())
    {
        player->SetPushing(false);
        stateMachine->ChangeState(player, new StateIdle());
    }
}
