#pragma once
#include "../CharaStateBase.h"
#include "../../Player.h"

class StateRun : public CharaStateBase
{
public:
    void Init(Player* player) override;
    void Update(Player* player, float dt) override;
};
