#include "StateMachine.h"
#include "../State/StateBase.h"

StateMachine::~StateMachine()
{
    delete current;
}

void StateMachine::Initialize(Player* player, StateBase* initState)
{
    ChangeState(player, initState);
}

void StateMachine::Update(Player* player, float dt)
{
    if (current)
        current->Update(player, dt);
}

void StateMachine::ChangeState(Player* player, StateBase* next)
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
        current->Enter(player);
    }
}
