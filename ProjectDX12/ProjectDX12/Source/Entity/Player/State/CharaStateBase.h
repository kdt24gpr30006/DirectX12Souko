#pragma once

class Player;
class CharaStateMachine;

class CharaStateBase
{
public:
    virtual ~CharaStateBase() {}

    virtual void Init(Player* player) {}
    virtual void Update(Player* player, float dt) = 0;
    virtual void Exit(Player* player) {}

    void SetStateMachine(CharaStateMachine* sm) { stateMachine = sm; }

protected:
    CharaStateMachine* stateMachine = nullptr;
};
