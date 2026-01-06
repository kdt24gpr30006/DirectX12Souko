#pragma once
#include "../Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"

/// <summary>
/// ゴールまで運ぶブロック
/// </summary>
class Block : public Entity
{
public:
    /// <summary>
    /// グリッド上の論理座標
    /// </summary>
    Int2 gridPos{ 0,0 };

    bool IsPushable() const override;

    /// <summary>
    /// グリッド → ワールド同期
    /// </summary>
    void SyncFromGrid(float cellSize = 1.0f);

protected:
    Math::Vector3 GetHalfSize() const override;
};
