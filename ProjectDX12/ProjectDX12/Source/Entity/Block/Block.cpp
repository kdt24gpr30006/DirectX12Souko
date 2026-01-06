#include "Block.h"
#include "Math/Vector3/Vector3.h"

bool Block::IsPushable() const
{
    return true;
}

void Block::SyncFromGrid(float cellSize)
{
    SetPosition({
        gridPos.x * cellSize,
        0.0f,
        gridPos.y * cellSize
        });
}

Math::Vector3 Block::GetHalfSize() const
{
    // プレイヤーとほぼ同サイズ
    return { 0.45f, 0.45f, 0.45f };
}
