#pragma once

/// <summary>
/// �Z���̃^�C�v
/// </summary>
enum class CellType
{
	None,       // ステージ外（描画なし、移動不可）
	Empty,
	Wall,
	Explosion,
	Goal,
};

/// <summary>
/// 移動結果
/// </summary>
enum class MoveResult
{
	Moved,      // 移動成功
	Blocked,    // 移動不可
};

/// <summary>
/// �ړ���̌���
/// </summary>
enum class MoveEndResult
{
	None,
	Exploded,
	Goal,
};