#pragma once

/// <summary>
/// セルのタイプ
/// </summary>
enum class CellType
{
	Empty,
	Wall,
	Explosion,
};

/// <summary>
/// 移動先の結果
/// </summary>
enum class MoveResult
{
	Moved,
	Blocked,
	Explosion,
};

/// <summary>
/// int型の2次元ベクトル
/// </summary>
struct Int2 
{
    int x;
    int y;

    /// <summary>
    /// Int2同士の足し算
    /// </summary>
    /// <param name="rhs"></param>
    /// <returns></returns>
    Int2 operator+(const Int2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    /// <summary>
    /// Int2同士の数値が等しいか
    /// </summary>
    /// <param name="rhs"></param>
    /// <returns></returns>
    bool operator==(const Int2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};