#pragma once
#include "SokobanGame.h"

namespace Sokoban
{
    // サンプルレベル1（基本的な倉庫番）
    inline LevelData CreateLevel1()
    {
        LevelData level;
        level.width = 7;
        level.height = 7;

        // レベルマップ定義
        // W=Wall, F=Floor, G=Goal
        // プレイヤー初期位置: P
        // ブロック初期位置: B
        //
        // マップ（グリッド座標 Y=0が下）
        //
        // Y=6: W W W W W W W
        // Y=5: W F F F F F W
        // Y=4: W F W W F F W
        // Y=3: W F B G F F W
        // Y=2: W F B G F F W
        // Y=1: W P F F F F W
        // Y=0: W W W W W W W

        level.cells.resize(level.width * level.height, CellType::Floor);

        auto setCell = [&](int x, int y, CellType type) {
            level.cells[y * level.width + x] = type;
        };

        // 外壁
        for (int x = 0; x < level.width; ++x)
        {
            setCell(x, 0, CellType::Wall);
            setCell(x, level.height - 1, CellType::Wall);
        }
        for (int y = 0; y < level.height; ++y)
        {
            setCell(0, y, CellType::Wall);
            setCell(level.width - 1, y, CellType::Wall);
        }

        // 内部の壁
        setCell(2, 4, CellType::Wall);
        setCell(3, 4, CellType::Wall);

        // ゴール位置
        setCell(3, 2, CellType::Goal);
        setCell(3, 3, CellType::Goal);
        level.goalPositions = { {3, 2}, {3, 3} };

        // プレイヤー初期位置
        level.playerStart = { 1, 1 };

        // ブロック初期位置
        level.blockStarts = { {2, 2}, {2, 3} };

        return level;
    }

    // サンプルレベル2（少し難しめ）
    inline LevelData CreateLevel2()
    {
        LevelData level;
        level.width = 9;
        level.height = 9;

        level.cells.resize(level.width * level.height, CellType::Floor);

        auto setCell = [&](int x, int y, CellType type) {
            level.cells[y * level.width + x] = type;
        };

        // 外壁
        for (int x = 0; x < level.width; ++x)
        {
            setCell(x, 0, CellType::Wall);
            setCell(x, level.height - 1, CellType::Wall);
        }
        for (int y = 0; y < level.height; ++y)
        {
            setCell(0, y, CellType::Wall);
            setCell(level.width - 1, y, CellType::Wall);
        }

        // 内部の壁（L字型）
        setCell(3, 3, CellType::Wall);
        setCell(3, 4, CellType::Wall);
        setCell(3, 5, CellType::Wall);
        setCell(4, 3, CellType::Wall);
        setCell(5, 3, CellType::Wall);

        // ゴール位置
        setCell(6, 6, CellType::Goal);
        setCell(7, 6, CellType::Goal);
        setCell(6, 7, CellType::Goal);
        level.goalPositions = { {6, 6}, {7, 6}, {6, 7} };

        // プレイヤー初期位置
        level.playerStart = { 1, 1 };

        // ブロック初期位置
        level.blockStarts = { {2, 2}, {2, 4}, {4, 2} };

        return level;
    }
}
