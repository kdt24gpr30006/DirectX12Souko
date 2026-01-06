#pragma once

class Player;
class StateBase;

class StateMachine
{
public:
    ~StateMachine();

    void Initialize(Player* player, StateBase* initState);
    void Update(Player* player, float dt);
    void ChangeState(Player* player, StateBase* next);

private:
    StateBase* current = nullptr;
};
