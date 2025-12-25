#pragma once
#include "GameTypes.h"
#include <vector>
#include "../Entity/Entity.h"

class Stage {
private:

    // グリッドのサイズ
    const static int GRID_SIZE = 9;

    // グリッドの情報
    CellType grid[GRID_SIZE][GRID_SIZE];

    // ブロック
    std::vector<Block> blocks;

public:
    void init();

    /// <summary>
    /// 指定座標の情報を取得
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    CellType getCellType(Int2 p) const;

    /// <summary>
    /// 他のブロックと干渉していないか
    /// </summary>
    /// <param name="p"></param>
    /// <returns></returns>
    Block* getBlockAt(Int2 p);

    /// <summary>
    /// ブロックの移動
    /// 中で壁やブロックのチェックを行う
    /// </summary>
    /// <param name="block"></param>
    /// <param name="dir"></param>
    /// <returns></returns>
    MoveResult moveBlock(Block& block, Int2 dir);
};