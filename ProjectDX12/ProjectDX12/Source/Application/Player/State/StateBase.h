#pragma once

class Player;
class StateMachine;

/// <summary>
/// プレイヤー用ステートの基底クラス
/// </summary>
class StateBase
{
public:
	StateBase();
	virtual ~StateBase();

	/// <summary>
	/// ステート初期化
	/// </summary>
	/// <param name="player"></param>
	/// <returns>true:成功</returns>
	virtual bool Initialize(Player* player) = 0;

	/// <summary>
	/// 解放処理
	/// </summary>
	/// <remarks>必要なら継承先でオーバーライド</remarks>
	virtual void Release(){}

	/// <summary>
	/// ステート更新処理
	/// </summary>
	/// <param name="player"></param>
	/// <param name="deltaTime"></param>
	virtual void Process(Player* player, const float deltaTime) = 0;

	/// <summary>
	/// ステート遷移条件のチェック
	/// </summary>
	/// <returns>true:遷移条件を満たす</returns>
	virtual bool CheckCondition(Player* player) = 0;

	/// <summary>
	/// 次の遷移先のステートを取得
	/// </summary>
	/// <returns>次のステート 例:new NextState()</returns>
	virtual StateBase* GetNextState() = 0;


	void SetStateMachine(StateMachine* stateMachine);

private:

	StateMachine* stateMachine;
};

