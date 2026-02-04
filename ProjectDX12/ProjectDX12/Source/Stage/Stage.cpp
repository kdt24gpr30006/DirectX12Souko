#include "Stage.h"
#include "../Stage/GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"
#include <Math/Vector3/Vector3.h>
#include "../Application/Field/Field.h"
#include <memory>
#include <utility>
#include <cmath>

Stage::~Stage()
{
    Release();
}

void Stage::Init(int stageNumber)
{
    // グリッド初期化
    for (int y = 0; y < GRID_SIZE; ++y)
    {
        for (int x = 0; x < GRID_SIZE; ++x)
        {
            grid[y][x] = CellType::Empty;
        }
    }

    // 外壁（全ステージ共通）
    for (int i = 0; i < GRID_SIZE; ++i)
    {
        grid[0][i] = CellType::Wall;
        grid[GRID_SIZE - 1][i] = CellType::Wall;
        grid[i][0] = CellType::Wall;
        grid[i][GRID_SIZE - 1] = CellType::Wall;
    }

    // ブロック配置用ラムダ
    blocks.clear();
    auto createBlock = [&](int x, int y) {
        auto block = std::make_unique<Block>();
        block->Init();
        block->SetGridPos({ x, y });
        block->SetPosition(GridToWorld({ x, y }));
        blocks.push_back(std::move(block));
    };

    // ステージ別レイアウト
    switch (stageNumber)
    {
    case 1:
        // ステージ1（チュートリアル）
        // プレイヤー開始位置
        playerStartPos = { 1, 1 };

        // ブロック配置（2個）
        createBlock(2, 2);
        createBlock(3, 3);

        // ゴール位置（2個）
        grid[5][3] = CellType::Goal;
        grid[5][4] = CellType::Goal;
        break;

    case 2:
        // ステージ2（現行ステージ）
        // プレイヤー開始位置
        playerStartPos = { 1, 1 };

        // 内壁
        grid[3][3] = CellType::Wall;
        grid[3][4] = CellType::Wall;
        grid[5][5] = CellType::Wall;

        // ブロック配置（3個）
        createBlock(2, 2);
        createBlock(3, 2);
        createBlock(3, 5);

        // ゴール位置（3個）
        grid[6][2] = CellType::Goal;
        grid[6][3] = CellType::Goal;
        grid[6][4] = CellType::Goal;

        // 爆発エリア
        grid[6][1] = CellType::Explosion;
        grid[7][7] = CellType::Explosion;
        break;

    case 3:
    default:
        // ステージ3（上級）
        // プレイヤー開始位置
        playerStartPos = { 1, 1 };

        // 内壁（L字型）
        grid[3][3] = CellType::Wall;
        grid[3][4] = CellType::Wall;
        grid[3][5] = CellType::Wall;
        grid[4][3] = CellType::Wall;
        grid[5][3] = CellType::Wall;

        // ブロック配置（4個）
        createBlock(2, 2);
        createBlock(2, 5);
        createBlock(4, 5);
        createBlock(4, 7);

        // ゴール位置（4個）
        grid[6][5] = CellType::Goal;
        grid[6][6] = CellType::Goal;
        grid[6][7] = CellType::Goal;
        grid[7][4] = CellType::Goal;

        // 爆発エリア
        grid[2][6] = CellType::Explosion;
        grid[8][5] = CellType::Explosion;
        break;
    }

    // フラグ初期化
    bHasExplosion = false;
    bHasGoal = false;

    // フィールド生成
    field = new Field();
    field->Init(this);
}

void Stage::Update(float deltaTime)
{
    for (auto& block : blocks)
    {
        block->Update(deltaTime, *this);

        if (block->HasMoveResult())
        {
            auto result = block->ConsumeMoveResult();
            if (result == MoveEndResult::Exploded)
            {
                OnBlockExploded();
            }
			else if (result == MoveEndResult::Goal)
			{
				OnBlockGoal();
			}
        }
    }
}

void Stage::Draw()
{
    if (field)
    {
        field->Render();
    }

    for (auto& block : blocks)
    {
        block->Draw();
    }
}

void Stage::Release()
{
    blocks.clear();

    if (field)
    {
        field->Release();
        delete field;
        field = nullptr;
    }
}

Math::Vector3 Stage::GridToWorld(const Int2& p) const
{
    const float half = CELL_SIZE * 0.5f;

    return {
        p.x * CELL_SIZE + half,
        0.0f,
        p.y * CELL_SIZE + half
    };
}

Int2 Stage::WorldToGrid(const Math::Vector3& pos) const
{
    return {
        static_cast<int>(std::floor(pos.x / CELL_SIZE)),
        static_cast<int>(std::floor(pos.z / CELL_SIZE))
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
        if (b->GetGridPos() == p)
            return b.get();
    }
    return nullptr;
}

MoveResult Stage::TryPush(Block& block, const Int2& dir)
{
    // ゴール上にあるブロックは動かせない
    if (GetCellType(block.GetGridPos()) == CellType::Goal)
        return MoveResult::Blocked;

    const Int2 next = block.GetGridPos() + dir;

    // 壁チェック
    if (GetCellType(next) == CellType::Wall)
        return MoveResult::Blocked;

    // 他ブロックチェック
    if (GetBlockAt(next) != nullptr)
        return MoveResult::Blocked;

    // 移動開始
    block.StartMove(dir, *this);
    return MoveResult::Moved;
}

void Stage::OnBlockExploded()
{
    bHasExplosion = true;
}

void Stage::OnBlockGoal()
{
    // 全ブロックがゴール上にあるかチェック
    bHasGoal = IsCleared();
}

bool Stage::IsCleared() const
{
    // 全ブロックがゴールセル上にあるか
    for (const auto& block : blocks)
    {
        CellType cell = GetCellType(block->GetGridPos());
        if (cell != CellType::Goal)
        {
            return false;
        }
    }
    return !blocks.empty();
}

bool Stage::HasExplosion() const
{
    return bHasExplosion;
}

bool Stage::HasGoal() const
{
    return bHasGoal;
}

bool Stage::IsInside(const Int2& p) const
{
    return
        p.x >= 0 && p.x < GRID_SIZE &&
        p.y >= 0 && p.y < GRID_SIZE;
}
