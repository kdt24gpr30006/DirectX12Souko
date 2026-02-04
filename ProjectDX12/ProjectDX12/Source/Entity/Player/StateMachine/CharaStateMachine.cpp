#include "CharaStateMachine.h"
#include "../State/CharaStateBase.h"

CharaStateMachine::~CharaStateMachine()
{
    delete current;
}

void CharaStateMachine::Init(Player* player, CharaStateBase* initState)
{
    ChangeState(player, initState);
}

void CharaStateMachine::Update(Player* player, float dt)
{
    if (current)
        current->Update(player, dt);
}

void CharaStateMachine::ChangeState(Player* player, CharaStateBase* next)
{
    if (current)
    {
        current->Exit(player);
        delete current;
    }

    current = next;

    if (current)
    {
        current->SetStateMachine(this);
        current->Init(player);
    }
}
