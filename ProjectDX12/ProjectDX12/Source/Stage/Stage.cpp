#include "Stage.h"
#include "StageLoader.h"
#include "../Stage/GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"
#include <Math/Vector3/Vector3.h>
#include "../Application/Field/Field.h"
#include <memory>
#include <utility>
#include <cmath>
#include <string>

Stage::~Stage()
{
    Release();
}

void Stage::Init(int stageNumber)
{
    // CSVファイルからステージデータを読み込み
    std::string filePath = "Assets/Stages/stage" + std::to_string(stageNumber) + ".csv";
    StageData data = StageLoader::Load(filePath);

    // グリッドサイズ設定
    gridWidth = data.width;
    gridHeight = data.height;

    // グリッドデータをコピー
    grid = std::move(data.cells);

    // プレイヤー開始位置
    playerStartPos = data.playerStart;

    // ブロック配置
    blocks.clear();
    for (const auto& blockPos : data.blockStarts)
    {
        auto block = std::make_unique<Block>();
        block->Init();
        block->SetGridPos(blockPos);
        block->SetPosition(GridToWorld(blockPos));
        block->UpdateCollider();
        blocks.push_back(std::move(block));
    }

    // ブロック初期位置を保存
    initialBlockPositions.clear();
    for (const auto& block : blocks)
    {
        BlockInitData initData;
        initData.gridPos = block->GetGridPos();
        initData.worldPos = block->GetPosition();
        initialBlockPositions.push_back(initData);
    }

    // フラグ初期化
    bHasExplosion = false;
    bHasGoal = false;

    // 壁コライダー生成（カメラ衝突判定用に高さを十分に取る）
    constexpr float WALL_HEIGHT = 50.0f;
    wallColliders.clear();
    for (int y = 0; y < gridHeight; ++y)
    {
        for (int x = 0; x < gridWidth; ++x)
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
    const Int2 next = block.GetGridPos() + dir;

    // 壁またはステージ外チェック
    CellType nextCell = GetCellType(next);
    if (nextCell == CellType::Wall || nextCell == CellType::None)
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
        p.x >= 0 && p.x < gridWidth &&
        p.y >= 0 && p.y < gridHeight;
}
