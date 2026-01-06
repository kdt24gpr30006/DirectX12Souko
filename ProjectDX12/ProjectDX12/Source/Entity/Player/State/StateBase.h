#pragma once

class Player;
class StateMachine;

class StateBase
{
public:
    virtual ~StateBase() {}

    virtual void Enter(Player* player) {}
    virtual void Update(Player* player, float dt) = 0;
    virtual void Exit(Player* player) {}

    void SetStateMachine(StateMachine* sm) { stateMachine = sm; }

protected:
    StateMachine* stateMachine = nullptr;
};
