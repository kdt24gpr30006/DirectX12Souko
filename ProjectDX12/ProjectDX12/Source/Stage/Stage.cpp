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
        block->UpdateCollider();
        blocks.push_back(std::move(block));
    };

    // ステージ別レイアウト
    switch (stageNumber)
    {
    case 1:
        // ステージ1（チュートリアル）
        // プレイヤー開始位置
        playerStartPos = { 4, 2 };

        // 内壁
        grid[5][4] = CellType::Wall;

        // ブロック配置
        createBlock(4, 4);

        // ゴール位置
        grid[6][4] = CellType::Goal;

        // 爆発エリア
        grid[4][5] = CellType::Explosion;
        break;

    case 2:
        // ステージ2（現行ステージ）
        // プレイヤー開始位置
        playerStartPos = { 4, 4 };

        // 内壁
        grid[3][4] = CellType::Wall;
        grid[5][3] = CellType::Wall;
        grid[5][5] = CellType::Wall;

        // ブロック配置
        createBlock(3, 3);
        createBlock(5, 3);

        // ゴール位置
        grid[5][4] = CellType::Goal;
        grid[5][6] = CellType::Goal;

        // 爆発エリア
        grid[3][2] = CellType::Explosion;
        grid[6][1] = CellType::Explosion;
        grid[5][7] = CellType::Explosion;
        break;

    case 3:
    default:
        // ステージ3（上級）
        // プレイヤー開始位置
        playerStartPos = { 4, 4 };

        // 内壁
        grid[2][4] = CellType::Wall;
        grid[3][4] = CellType::Wall;
        grid[5][4] = CellType::Wall;
        grid[6][4] = CellType::Wall;

        // ブロック配置
        createBlock(2, 3);
        createBlock(2, 5);
        createBlock(6, 3);
        createBlock(6, 5);

        // ゴール位置
        grid[1][2] = CellType::Goal;
        grid[2][2] = CellType::Goal;
        grid[7][6] = CellType::Goal;
        grid[6][6] = CellType::Goal;

        // 爆発エリア
        grid[4][2] = CellType::Explosion;
        grid[4][6] = CellType::Explosion;
        break;
    }

    // ブロック初期位置を保存
    initialBlockPositions.clear();
    for (const auto& block : blocks)
    {
        BlockInitData data;
        data.gridPos = block->GetGridPos();
        data.worldPos = block->GetPosition();
        initialBlockPositions.push_back(data);
    }

    // フラグ初期化
    bHasExplosion = false;
    bHasGoal = false;

    // 壁コライダー生成（カメラ衝突判定用に高さを十分に取る）
    constexpr float WALL_HEIGHT = 50.0f;
    wallColliders.clear();
    for (int y = 0; y < GRID_SIZE; ++y)
    {
        for (int x = 0; x < GRID_SIZE; ++x)
        {
            if (grid[y][x] == CellType::Wall)
            {
                AABBCollider collider;
                Math::Vector3 pos = GridToWorld({ x, y });
                pos.y = WALL_HEIGHT * 0.5f;
                collider.SetCenter(pos);
                collider.SetVolume(Math::Vector3(CELL_SIZE, WALL_HEIGHT, CELL_SIZE));
                wallColliders.push_back(collider);
            }
        }
    }

    // フィールド生成
    field = new Field();
    field->Init(this);
}

void Stage::Reset()
{
    bHasExplosion = false;
    bHasGoal = false;

    for (size_t i = 0; i < blocks.size() && i < initialBlockPositions.size(); ++i)
    {
        blocks[i]->ResetToInitial(
            initialBlockPositions[i].gridPos,
            initialBlockPositions[i].worldPos
        );
    }
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
