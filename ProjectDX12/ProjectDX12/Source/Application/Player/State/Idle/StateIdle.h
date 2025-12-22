#pragma once
#include "../StateBase.h"

class StateIdle : public StateBase
{
	/// <summary>
	/// 並行移動入力
	/// </summary>
	static constexpr int TRANSLATION_KEY = 0x01;

	/// <summary>
	/// ジャンプ入力
	/// </summary>
	static constexpr int JUMP_KEY = 0x02;
public:
	/// <summary>
	/// ステート初期化
	/// </summary>
	/// <param name="player"></param>
	/// <returns>true:成功</returns>
	virtual bool Initialize(Player* player) override;

	/// <summary>
	/// 解放処理
	/// </summary>
	/// <remarks>必要なら継承先でオーバーライド</remarks>
	virtual void Release() override;

	/// <summary>
	/// ステート更新処理
	/// </summary>
	/// <param name="player"></param>
	/// <param name="deltaTime"></param>
	virtual void Process(Player* player, const float deltaTime) override;

	/// <summary>
	/// ステート遷移条件のチェック
	/// </summary>
	/// <returns>true:遷移条件を満たす</returns>
	virtual bool CheckCondition(Player* player) override;

	/// <summary>
	/// 次の遷移先のステートを取得
	/// </summary>
	/// <returns>次のステート 例:new NextState()</returns>
	virtual StateBase* GetNextState() override;

private:

	/// <summary>
	/// 経過時間
	/// </summary>
	float ElapsedTime;

	int InputFlag;
};

