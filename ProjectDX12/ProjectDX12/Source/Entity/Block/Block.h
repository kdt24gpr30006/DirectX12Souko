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
    float moveSpeed = 3.0f;

public:

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime"></param>
    /// <param name="stage"></param>
    void Update(float deltaTime, Stage& stage);

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

protected:

};
