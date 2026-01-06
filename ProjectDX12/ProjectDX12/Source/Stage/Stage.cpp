#include "Stage.h"
#include "../Stage/GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"
#include <Math/Vector3/Vector3.h>

void Stage::Initialize()
{
    // グリッド初期化
    for (int y = 0; y < GRID_SIZE; ++y)
    {
        for (int x = 0; x < GRID_SIZE; ++x)
        {
            grid[y][x] = CellType::Empty;
        }
    }

    // ステージ1 仮配置
    grid[2][5] = CellType::Wall;
    grid[3][3] = CellType::Wall;
    grid[5][3] = CellType::Wall;
    grid[5][4] = CellType::Wall;
    grid[6][6] = CellType::Wall;

    grid[3][4] = CellType::Explosion;
    grid[4][3] = CellType::Explosion;
    grid[5][5] = CellType::Explosion;

    // ブロック配置
    blocks.clear();

    Block b1;
    b1.SetGridPos({ 1, 1 });
    b1.SetPosition(GridToWorld(b1.GetGridPos()));
    blocks.push_back(b1);

    bHasExplosion = false;
}

Math::Vector3 Stage::GridToWorld(const Int2& p) const
{
    return {
        p.x * CELL_SIZE,
        0.0f,
        p.y * CELL_SIZE
    };
}

CellType Stage::GetCellType(const Int2& p) const
{
    if (!IsInside(p))
        return CellType::Wall;

    return grid[p.y][p.x];
}

Block* Stage::GetBlockAt(const Int2& p)
{
    for (auto& b : blocks)
    {
        if (b.GetGridPos() == p)
            return &b;
    }
    return nullptr;
}

MoveResult Stage::TryPush(Block& block, const Int2& dir)
{
    const Int2 next = block.GetGridPos() + dir;

    // 壁
    if (GetCellType(next) == CellType::Wall)
        return MoveResult::Blocked;

    // 他ブロック
    if (GetBlockAt(next) != nullptr)
        return MoveResult::Blocked;

    // 移動開始（完了時に爆発判定）
    block.StartMove(dir, *this);
    return MoveResult::Explosion;
}

void Stage::OnBlockExploded(Block& block)
{
    bHasExplosion = true;
}

bool Stage::HasExplosion() const
{
    return bHasExplosion;
}

bool Stage::IsInside(const Int2& p) const
{
    return
        p.x >= 0 && p.x < GRID_SIZE &&
        p.y >= 0 && p.y < GRID_SIZE;
}
