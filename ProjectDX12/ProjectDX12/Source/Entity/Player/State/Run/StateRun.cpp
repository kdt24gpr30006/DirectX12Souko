#include "StateRun.h"
#include "../../Player.h"
#include "../Idle/StateIdle.h"
#include "../Push/StatePush.h"
#include "System/Input/Input.h"
#include "../../StateMachine/StateMachine.h"

void StateRun::Enter(Player* player)
{
}

void StateRun::Update(Player* player, float dt)
{
    player->PlayAnimation("Run", dt, true);

    // キーボード取得
    auto& kb = System::Input::GetInstance()->Keyboard();

    // 移動入力を取得
    Math::Vector3 move{};
    if (kb.IsPress('W')) move.z += 1;
    if (kb.IsPress('S')) move.z -= 1;
    if (kb.IsPress('A')) move.x -= 1;
    if (kb.IsPress('D')) move.x += 1;

    // 入力がなければIdleに
    if (move.x == 0 && move.z == 0)
    {
        stateMachine->ChangeState(player, new StateIdle());
        return;
    }

    // 移動処理
    move.Normalize();
    player->SetPosition(player->GetPosition() + move * Player::MoveSpeed * dt);

    // Push判定
    if (kb.IsPush('E'))
    {
        Block* block = nullptr;
        Int2 dir{};
        if (player->CanPush(block, dir))
        {
            stateMachine->ChangeState(player, new StatePush(block, dir));
        }
    }
}
