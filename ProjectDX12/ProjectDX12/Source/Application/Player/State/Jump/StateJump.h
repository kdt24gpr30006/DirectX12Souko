#pragma once
#include "../StateBase.h"
#include "Math/Vector3/Vector3.h"


class StateJump : public StateBase
{
public:
	StateJump();
	~StateJump();

	/// <summary>
	/// ステート初期化
	/// </summary>
	/// <param name="player"></param>
	/// <returns>true:成功</returns>
	virtual bool Initialize(Player* player) override;

	/// <summary>
	/// 解放処理
	/// </summary>
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
	Math::Vector3 Velocity;
	Math::Vector3 GravityAccel;
};

