#include "StateIdle.h"
#include "../../Player.h"
#include "../Run/StateRun.h"
#include "System/Input/Input.h"
#include "../../StateMachine/CharaStateMachine.h"
#include "../../../../Application/CameraWork/CameraWork.h"

void StateIdle::Init(Player* player)
{
}

void StateIdle::Update(Player* player, float dt)
{
    // アニメーション再生
    player->PlayAnimation("Idle", dt, true);

    // トップダウン視点では移動不可
    CameraWork* cam = player->GetCameraWork();
    if (cam && cam->GetCameraMode() == CameraMode::TopDown)
    {
        return;
    }

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
