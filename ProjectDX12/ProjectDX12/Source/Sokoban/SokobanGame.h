#pragma once
#include <vector>
#include <cstdint>
#include "SokobanTypes.h"
#include "Math/Int2/Int2.h"

namespace Sokoban
{
    // レベルデータ
    struct LevelData
    {
        int width = 0;
        int height = 0;
        std::vector<CellType> cells;    // [y * width + x]
        Int2 playerStart{ 0, 0 };
        std::vector<Int2> blockStarts;
        std::vector<Int2> goalPositions;
    };

    // ゲーム状態
    class SokobanGame
    {
    public:
        SokobanGame() = default;

        // レベルをロード
        void LoadLevel(const LevelData& level);

        // ゲームをリセット（現在のレベルを最初から）
        void Reset();

        // プレイヤーを移動（ブロック押しも含む）
        MoveResult TryMove(Direction dir);

        // クリア判定
        bool IsCleared() const;

        // Getter
        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
        CellType GetCell(int x, int y) const;
        CellType GetCell(const Int2& pos) const { return GetCell(pos.x, pos.y); }
        const Int2& GetPlayerPos() const { return playerPos; }
        const std::vector<Int2>& GetBlockPositions() const { return blockPositions; }
        const std::vector<Int2>& GetGoalPositions() const { return goalPositions; }

        // 指定位置にブロックがあるか
        bool HasBlockAt(const Int2& pos) const;

        // 範囲内か
        bool IsInside(const Int2& pos) const;

    private:
        int width = 0;
        int height = 0;
        std::vector<CellType> cells;
        Int2 playerPos{ 0, 0 };
        std::vector<Int2> blockPositions;
        std::vector<Int2> goalPositions;

        // 初期状態（リセット用）
        Int2 initialPlayerPos{ 0, 0 };
        std::vector<Int2> initialBlockPositions;
    };
}
