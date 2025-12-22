#pragma once

class Player;
class StateBase;

/// <summary>
/// プレイヤー用ステートマシンクラス
/// </summary>
class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="player"></param>
	/// <param name="initState">初期ステート</param>
	/// <returns>true:成功</returns>
	bool Initialize(Player* player, StateBase* initState);

	/// <summary>
	/// 解放
	/// </summary>
	void Release();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="player"></param>
	/// <param name="deltaTime"></param>
	void Process(Player* player, const float deltaTime);

private:
	/// <summary>
	/// 次のステートへ遷移
	/// </summary>
	/// <param name="nextState"></param>
	void ChangeState(Player* player, StateBase* nextState);

	/// <summary>
	/// 現在のステート
	/// </summary>
	StateBase* CurrentState;
};

