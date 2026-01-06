#include "Block.h"
#include "Math/Vector3/Vector3.h"
#include "../../Stage/Stage.h"
#include "../../Stage/GameTypes.h"
#include <Math/Int2/Int2.h>
#include <algorithm>

void Block::Update(float deltaTime, Stage& stage)
{
    if (!bIsMoving)
        return;

    // 目標までのベクトル
    Math::Vector3 current = GetPosition();
    Math::Vector3 dir = targetWorldPos - current;

    float dist = dir.Length();

    // 到達判定
    if (dist < 0.001f)
    {
        FinishMove(stage);
        return;
    }

    dir.Normalize();

    // 移動量が目標を超えないように
    float move = moveSpeed * deltaTime;
    move = std::min(move, dist);

    SetPosition(current + dir * move);
}

const Int2& Block::GetGridPos() const
{
    return gridPos;
}

void Block::SetGridPos(const Int2& pos)
{
    gridPos = pos;
}

void Block::StartMove(const Int2& dir, const Stage& stage)
{
    targetGridPos = gridPos + dir;

    startWorldPos = GetPosition();
    targetWorldPos = stage.GridToWorld(targetGridPos);

    bIsMoving = true;
}

MoveEndResult Block::FinishMove(const Stage& stage)
{
    gridPos = targetGridPos;
    bIsMoving = false;

    SetPosition(stage.GridToWorld(gridPos));

    if (stage.GetCellType(gridPos) == CellType::Explosion)
    {
        return MoveEndResult::Exploded;
    }

    return MoveEndResult::None;

    // こんな感じで呼び出す
    // MoveEndResult result = block.FinishMove(*this);
    // 
    // if (result == MoveEndResult::Exploded)
    // {
    //     OnBlockExploded(block);
    // }
}