#pragma once
#include "../StateBase.h"
#include "Math/Int2/Int2.h"
#include "../../Player.h"

class Block;

class StatePush : public StateBase
{
public:
    StatePush(Block* block, const Int2& dir);

    void Enter(Player* player) override;
    void Update(Player* player, float dt) override;

private:
    // 押すブロック
    Block* block = nullptr;
    // 押す方向
    Int2 dir{};
    // 押し終わったか
    bool isEnd = false;
};
