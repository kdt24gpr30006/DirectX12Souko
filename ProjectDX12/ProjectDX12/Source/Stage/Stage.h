#pragma once
#include <vector>
#include "GameTypes.h"
#include "../Entity/Block/Block.h"

/// <summary>
/// ステージ（グリッド＋ブロック管理）
/// </summary>
class Stage
{
public:
    static constexpr int GRID_SIZE = 9;
    static constexpr float CELL_SIZE = 1.0f;

public:
    Stage() = default;

    /// <summary>
    /// ステージ初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// グリッド情報取得
    /// </summary>
    CellType GetCellType(const Int2& p) const;

    /// <summary>
    /// 指定グリッドにあるブロック取得
    /// </summary>
    Block* GetBlockAt(const Int2& p);

    /// <summary>
    /// 押し処理（1マス単位）
    /// </summary>
    MoveResult TryPush(Block& block, const Int2& dir);

    /// <summary>
    /// 全ブロック取得
    /// </summary>
    const std::vector<Block>& GetBlocks() const { return blocks; }

private:
    CellType grid[GRID_SIZE][GRID_SIZE]{};
    std::vector<Block> blocks;

private:
    bool IsInside(const Int2& p) const;
};
