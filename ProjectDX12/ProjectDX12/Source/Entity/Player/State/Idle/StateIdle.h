#pragma once
#include "../StateBase.h"
#include "../../Player.h"

class StateIdle : public StateBase
{
public:
    void Enter(Player* player) override;
    void Update(Player* player, float dt) override;
};
