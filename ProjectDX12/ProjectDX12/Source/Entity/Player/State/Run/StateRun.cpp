#include "StateRun.h"
#include "../../Player.h"
#include "../Idle/StateIdle.h"
#include "System/Input/Input.h"
#include "../../StateMachine/CharaStateMachine.h"
#include "../../../../Application/CameraWork/CameraWork.h"
#include <Math/Vector3/Vector3.h>

void StateRun::Init(Player* player)
{
}

void StateRun::Update(Player* player, float dt)
{
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

    // カメラ方向を基準にワールド座標の移動方向を計算
    CameraWork* cam = player->GetCameraWork();
    Math::Vector3 move{ 0, 0, 0 };

    if (cam)
    {
        Math::Vector3 camForward = cam->GetForwardXZ();
        Math::Vector3 camRight = cam->GetRightXZ();

        move = camForward * inputForward + camRight * inputRight;
        move.Normalize();
    }
    else
    {
        // カメラがない場合はワールド座標そのまま
        move = { inputRight, 0.0f, inputForward };
        move.Normalize();
    }

    // プレイヤーの向きを移動方向に合わせる
    player->SetFacingDirection(move);

    // 移動
    player->SetPosition(player->GetPosition() + move * Player::MoveSpeed * dt);
}
