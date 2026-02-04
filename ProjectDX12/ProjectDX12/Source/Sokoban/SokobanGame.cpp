#include "SokobanGame.h"
#include <algorithm>

namespace Sokoban
{
    void SokobanGame::LoadLevel(const LevelData& level)
    {
        width = level.width;
        height = level.height;
        cells = level.cells;
        goalPositions = level.goalPositions;

        // 初期状態を保存
        initialPlayerPos = level.playerStart;
        initialBlockPositions = level.blockStarts;

        Reset();
    }

    void SokobanGame::Reset()
    {
        playerPos = initialPlayerPos;
        blockPositions = initialBlockPositions;
    }

    MoveResult SokobanGame::TryMove(Direction dir)
    {
        if (dir == Direction::None)
            return MoveResult::Invalid;

        const Int2 offset = DirectionToOffset(dir);
        const Int2 targetPos = playerPos + offset;

        // 範囲外チェック
        if (!IsInside(targetPos))
            return MoveResult::Invalid;

        // 壁チェック
        if (GetCell(targetPos) == CellType::Wall)
            return MoveResult::Blocked;

        // ブロックがあるかチェック
        if (HasBlockAt(targetPos))
        {
            // ブロックの先の位置
            const Int2 pushTargetPos = targetPos + offset;

            // 押し先が範囲外
            if (!IsInside(pushTargetPos))
                return MoveResult::Blocked;

            // 押し先が壁
            if (GetCell(pushTargetPos) == CellType::Wall)
                return MoveResult::Blocked;

            // 押し先に別のブロックがある
            if (HasBlockAt(pushTargetPos))
                return MoveResult::Blocked;

            // ブロックを押す
            for (auto& blockPos : blockPositions)
            {
                if (blockPos == targetPos)
                {
                    blockPos = pushTargetPos;
                    break;
                }
            }

            // プレイヤーも移動
            playerPos = targetPos;
            return MoveResult::SuccessPush;
        }

        // 通常移動
        playerPos = targetPos;
        return MoveResult::Success;
    }

    bool SokobanGame::IsCleared() const
    {
        // すべてのブロックがゴール上にあるか
        for (const auto& blockPos : blockPositions)
        {
            bool onGoal = false;
            for (const auto& goalPos : goalPositions)
            {
                if (blockPos == goalPos)
                {
                    onGoal = true;
                    break;
                }
            }
            if (!onGoal)
                return false;
        }
        return !blockPositions.empty();
    }

    CellType SokobanGame::GetCell(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return CellType::Wall;
        return cells[y * width + x];
    }

    bool SokobanGame::HasBlockAt(const Int2& pos) const
    {
        for (const auto& blockPos : blockPositions)
        {
            if (blockPos == pos)
                return true;
        }
        return false;
    }

    bool SokobanGame::IsInside(const Int2& pos) const
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }
}
