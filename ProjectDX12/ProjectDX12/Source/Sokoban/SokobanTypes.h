#pragma once
#include "Math/Int2/Int2.h"

namespace Sokoban
{
    // グリッドセルの種類
    enum class CellType : uint8_t
    {
        Floor,  // 歩行可能な床
        Wall,   // 壁（移動不可）
        Goal,   // ゴール地点（床として歩行可能）
    };

    // 移動方向
    enum class Direction
    {
        None,
        Up,     // Z+方向（グリッドY+）
        Down,   // Z-方向（グリッドY-）
        Left,   // X-方向
        Right,  // X+方向
    };

    // 移動結果
    enum class MoveResult
    {
        Success,        // 移動成功
        SuccessPush,    // ブロックを押して移動成功
        Blocked,        // 壁またはブロックで移動不可
        Invalid,        // 範囲外
    };

    // 方向からグリッドオフセットを取得
    inline Int2 DirectionToOffset(Direction dir)
    {
        switch (dir)
        {
        case Direction::Up:    return { 0, 1 };
        case Direction::Down:  return { 0, -1 };
        case Direction::Left:  return { -1, 0 };
        case Direction::Right: return { 1, 0 };
        default:               return { 0, 0 };
        }
    }
}
