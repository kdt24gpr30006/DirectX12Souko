#pragma once
#include "../CharaStateBase.h"
#include "../../Player.h"

class StateIdle : public CharaStateBase
{
public:
    void Init(Player* player) override;
    void Update(Player* player, float dt) override;
};
