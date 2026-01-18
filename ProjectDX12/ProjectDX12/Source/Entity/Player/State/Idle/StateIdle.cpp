#include "StateIdle.h"
#include "../../Player.h"
#include "../Run/StateRun.h"
#include "System/Input/Input.h"
#include "../../StateMachine/StateMachine.h"

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
}
