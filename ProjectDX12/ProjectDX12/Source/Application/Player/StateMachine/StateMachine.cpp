#include "StateMachine.h"
#include "../State/StateBase.h"
#include "../Player.h"

StateMachine::StateMachine()
	: CurrentState(nullptr)
{
}
StateMachine::~StateMachine()
{
}

/// <summary>
/// 初期化
/// </summary>
/// <param name="player"></param>
/// <param name="initState">初期ステート</param>
/// <returns>true:成功</returns>
bool StateMachine::Initialize(Player* player, StateBase* initState)
{
	if (initState == nullptr)
	{
		//	TODO:警告ログを出す
		return false;
	}
	if (CurrentState != nullptr)
	{
		//	TODO:警告ログを出す
		return false;
	}

	ChangeState(player, initState);
	return true;
}

/// <summary>
/// 解放
/// </summary>
void StateMachine::Release()
{
	if (CurrentState != nullptr)
	{
		delete CurrentState;
		CurrentState = nullptr;
	}
}

/// <summary>
/// 更新処理
/// </summary>
/// <param name="player"></param>
/// <param name="deltaTime"></param>
void StateMachine::Process(Player* player, const float deltaTime)
{
	if (CurrentState == nullptr)
	{
		//	TODO:警告ログを出す
		return;
	}

	//	現在のステートの更新処理
	CurrentState->Process(player, deltaTime);
	const bool bNext = CurrentState->CheckCondition(player);
	if (bNext == true)
	{
		//	遷移条件を満たし次に遷移
		StateBase* nextState = CurrentState->GetNextState();
		ChangeState(player, nextState);
	}
}

/// <summary>
/// 次のステートへ遷移
/// </summary>
/// <param name="nextState"></param>
void StateMachine::ChangeState(Player* player, StateBase* nextState)
{
	if (CurrentState != nextState)
	{
		if (CurrentState != nullptr)
		{
			//	現在のステートと違う情報なら現在のステートを解放
			CurrentState->Release();
			delete CurrentState;
			CurrentState = nullptr;
		}
	}

	if (nextState == nullptr)
	{
		return;
	}

	//	次のステートの初期化
	const bool bInit = nextState->Initialize(player);
	if (bInit == false)
	{
		//	TODO:警告ログを出す
		nextState->Release();
		delete nextState;
		return;
	}

	nextState->SetStateMachine(this);
	CurrentState = nextState;
}