#include "Stage.h"

void Stage::init()
{
	// マップ初期化
	for (int y = 0; y < GRID_SIZE; y++)
	{
		for (int x = 0; x < GRID_SIZE; x++)
		{
			grid[y][x] = CellType::Empty;
		}
	}

	// ステージ１(一旦ここに)
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
	b1.pos = { 1, 1 };
	blocks.push_back(b1);
	Block b2;
	b2.pos = { 2, 1 };
	blocks.push_back(b2);

}

CellType Stage::getCellType(Int2 argPos) const
{
	// 範囲外は壁とする
	if (argPos.x < 0 || argPos.x >= GRID_SIZE || argPos.y < 0 || argPos.y >= GRID_SIZE)
		return CellType::Wall;

	return grid[argPos.y][argPos.x];
}

Block* Stage::getBlockAt(Int2 argPos)
{
	for (auto& b : blocks)
	{
		if (b.pos == argPos)
		{
			return &b;
		}
	}
	return nullptr;
}

MoveResult Stage::moveBlock(Block& block, Int2 dir)
{
	Int2 next = block.pos + dir;

	// 壁チェック
	if (getCellType(next) == CellType::Wall)
		return MoveResult::Blocked;

	// 他ブロックチェック
	if (getBlockAt(next) != nullptr)
	{
		return MoveResult::Blocked;
	}

	// 移動
	block.Move(dir);

	// 爆発判定
	if (getCellType(next) == CellType::Explosion)
		return MoveResult::Explosion;

	return MoveResult::Moved;
}