#include "StateIdle.h"
#include "../../Player.h"
#include "../Run/StateRun.h"
#include "../Push/StatePush.h"
#include "System/Input/Input.h"
#include "../../StateMachine/StateMachine.h"
#include <Math/Int2/Int2.h>

void StateIdle::Enter(Player* player)
{
}

void StateIdle::Update(Player* player, float dt)
{
    // アニメーション再生
    player->PlayAnimation("Idle", dt, true);

    // キーボード取得
    auto& kb = System::Input::GetInstance()->Keyboard();

	// 移動入力があればRunに
    if (kb.IsPress('W') || kb.IsPress('A') ||
        kb.IsPress('S') || kb.IsPress('D'))
    {
        stateMachine->ChangeState(player, new StateRun());
        return;
    }

    // Push判定
    if (kb.IsPush('E'))
    {
        // 押す用のブロックと方向を取得
        Block* block = nullptr;
        Int2 dir{};
        if (player->CanPush(block, dir))
        {
            stateMachine->ChangeState(player, new StatePush(block, dir));
        }
    }
}
