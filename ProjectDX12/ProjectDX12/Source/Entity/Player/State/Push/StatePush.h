#pragma once
#include "../CharaStateBase.h"
#include "Math/Int2/Int2.h"
#include "Math/Vector3/Vector3.h"

class Block;
class Player;

class StatePush : public CharaStateBase
{
public:
    StatePush(Block* block, const Int2& dir);

    void Init(Player* player) override;
    void Update(Player* player, float dt) override;

private:
    Block* block = nullptr;
    Int2 dir{};
    bool pushSucceeded = false;

    // プレイヤー移動用
    Math::Vector3 playerStartPos;
    Math::Vector3 playerTargetPos;
    static constexpr float PUSH_MOVE_SPEED = 20.0f;  // ブロックと同じ速度
};
