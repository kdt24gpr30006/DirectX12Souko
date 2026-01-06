#include "Stage.h"
#include "../Stage/GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"

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

    // 仮ステージ定義
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
    b1.gridPos = { 1,1 };
    b1.SyncFromGrid(CELL_SIZE);
    blocks.push_back(b1);

    Block b2;
    b2.gridPos = { 2,1 };
    b2.SyncFromGrid(CELL_SIZE);
    blocks.push_back(b2);
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
        if (b.gridPos == p)
            return &b;
    }
    return nullptr;
}

MoveResult Stage::TryPush(Block& block, const Int2& dir)
{
    const Int2 next = block.gridPos + dir;

    // 壁
    if (GetCellType(next) == CellType::Wall)
        return MoveResult::Blocked;

    // 他ブロック
    if (GetBlockAt(next) != nullptr)
        return MoveResult::Blocked;

    // グリッド更新
    block.gridPos = next;
    block.SyncFromGrid(CELL_SIZE);

    // 爆発
    if (GetCellType(next) == CellType::Explosion)
        return MoveResult::Explosion;

    return MoveResult::Moved;
}

bool Stage::IsInside(const Int2& p) const
{
    return
        p.x >= 0 && p.x < GRID_SIZE &&
        p.y >= 0 && p.y < GRID_SIZE;
}
