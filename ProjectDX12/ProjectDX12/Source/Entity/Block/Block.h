#pragma once
#include "../Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"
#include "../../Stage/GameTypes.h"

class Stage;

/// <summary>
/// ゴールまで運ぶブロック
/// </summary>
class Block : public Entity
{
	// グリッド上の論理座標
	Int2 gridPos{ 0, 0 };

	// 移動先のグリッド座標
	Int2 targetGridPos{ 0, 0 };

	// 移動開始地点
	Math::Vector3 startWorldPos;

	// 移動目標地点
	Math::Vector3 targetWorldPos;

	// 移動中か
	bool bIsMoving = false;

	// 移動速度
	const float MOVE_SPEED = 10.0f;

	// 移動結果
	MoveEndResult lastMoveResult = MoveEndResult::None;

public:

	Block();
	~Block() = default;

	// コピー禁止()
	Block(const Block&) = delete;
	Block& operator=(const Block&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="stage"></param>
	/// <returns></returns>
	bool Init() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="stage"></param>
	void Update(float deltaTime, const Stage& stage);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Release() override;

	/// <summary>
	/// グリッド位置のゲッター
	/// </summary>
	/// <returns></returns>
	const Int2& GetGridPos() const;

	/// <summary>
	/// グリッド位置のセッター
	/// </summary>
	/// <param name="pos"></param>
	void SetGridPos(const Int2& pos);

	/// <summary>
	/// 指定した方向への移動を開始
	/// </summary>
	/// <param name="dir"></param>
	void StartMove(const Int2& dir, const Stage& stage);

	/// <summary>
	/// 移動結果がnoneではないか
	/// </summary>
	/// <returns></returns>
	bool HasMoveResult() const
	{
		return lastMoveResult != MoveEndResult::None;
	}

	/// <summary>
	/// 移動結果を取得しリセットする
	/// </summary>
	/// <returns></returns>
	MoveEndResult ConsumeMoveResult()
	{
		MoveEndResult r = lastMoveResult;
		lastMoveResult = MoveEndResult::None;
		return r;
	}

	/// <summary>
	/// 移動完了　
	/// 結果を返す
	/// </summary>
	/// <param name="stage"></param>
	/// <returns></returns>
	MoveEndResult FinishMove(const Stage& stage);

	/// <summary>
	/// 移動中か
	/// </summary>
	/// <returns></returns>
	bool IsMoving() const { return bIsMoving; }
};
