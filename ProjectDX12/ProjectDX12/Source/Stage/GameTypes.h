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
	Blocked,
	Explosion,
};

/// <summary>
/// 移動後の結果
/// </summary>
enum class MoveEndResult
{
	None,
	Exploded
};