#include "StateIdle.h"
#include <Windows.h>
#include "../../Player.h"
#include "System/Input/Input.h"
#include "../Run/StateRun.h"
#include "../Jump/StateJump.h"

/// <summary>
/// ステート初期化
/// </summary>
/// <param name="player"></param>
/// <returns>true:成功</returns>
bool StateIdle::Initialize(Player* player)
{
	ElapsedTime = 0.0f;
	InputFlag = false;
	return true;
}

/// <summary>
/// 解放処理
/// </summary>
/// <remarks>必要なら継承先でオーバーライド</remarks>
void StateIdle::Release()
{

}

/// <summary>
/// ステート更新処理
/// </summary>
/// <param name="player"></param>
/// <param name="deltaTime"></param>
void StateIdle::Process(Player* player, const float deltaTime)
{
	ElapsedTime += deltaTime;
	player->Animate(EPlayerAnimationState::Idle, deltaTime);
}

/// <summary>
/// ステート遷移条件のチェック
/// </summary>
/// <returns>true:遷移条件を満たす</returns>
bool StateIdle::CheckCondition(Player* player)
{
	const System::Input* input = System::Input::GetInstance();
	const bool KeyW = input->Keyboard().IsPress('W');
	const bool KeyA = input->Keyboard().IsPress('A');
	const bool KeyS = input->Keyboard().IsPress('S');
	const bool KeyD = input->Keyboard().IsPress('D');
	//	移動入力のチェック
	if (KeyW == true ||
		KeyA == true ||
		KeyS == true ||
		KeyD == true)
	{
		InputFlag |= TRANSLATION_KEY;
		return true;
	}

	//	ジャンプ入力のチェック
	const bool KeySpace = input->Keyboard().IsPress(VK_SPACE);
	if (KeySpace == true)
	{
		InputFlag |= JUMP_KEY;
		return true;
	}

	return false;
}

/// <summary>
/// 次の遷移先のステートを取得
/// </summary>
/// <returns>次のステート 例:new NextState()</returns>
StateBase* StateIdle::GetNextState()
{
	if (InputFlag & TRANSLATION_KEY)
	{
		return new StateRun();
	}

	if (InputFlag & JUMP_KEY)
	{
		return new StateJump();
	}

	return nullptr;
}