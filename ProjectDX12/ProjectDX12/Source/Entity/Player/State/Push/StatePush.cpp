#include "StatePush.h"
#include "../../Player.h"
#include "../Idle/StateIdle.h"
#include "../../../../Stage/Stage.h"
#include "../../StateMachine/StateMachine.h"

StatePush::StatePush(Block* b, const Int2& d)
    : block(b), dir(d)
{
}

void StatePush::Enter(Player* player)
{
    isEnd = player->GetStage()->TryPush(*block, dir) != MoveResult::Blocked;
}

void StatePush::Update(Player* player, float dt)
{
    // アニメーション再生
    player->PlayAnimation("Push", dt, false);
 
    // 押し終わったらIdleに
    if (isEnd == true)
    {
        stateMachine->ChangeState(player, new StateIdle());
    }
}
