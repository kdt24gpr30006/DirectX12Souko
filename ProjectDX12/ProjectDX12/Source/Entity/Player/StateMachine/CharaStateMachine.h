#pragma once

class Player;
class CharaStateBase;

class CharaStateMachine
{
public:
    ~CharaStateMachine();

    void Init(Player* player, CharaStateBase* initState);
    void Update(Player* player, float dt);
    void ChangeState(Player* player, CharaStateBase* next);

private:
    CharaStateBase* current = nullptr;
};
