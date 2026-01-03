#include "StateMachine.h"
#include "../State/StateBase.h"

StateMachine::~StateMachine()
{
    if (current)
    {
        delete current;
        current = nullptr;
    }
}

void StateMachine::Initialize(Player* owner, StateBase* start)
{
    if (current != nullptr)
    {
        delete current;
    }

    // 初期ステートを設定
    current = start;
    if (current != nullptr)
    {
        current->SetStateMachine(this);
        current->Enter(owner);
    }
}

void StateMachine::Update(Player* owner, float dt)
{
    if (current != nullptr)
    {
        current->Update(owner, dt);
    }
}

void StateMachine::ChangeState(Player* owner, StateBase* next)
{
    if (current != nullptr)
    {
        current->Exit(owner);
        delete current;
    }

    // 次のステートに変更
    current = next;

    if (current != nullptr)
    {
        current->SetStateMachine(this);
        current->Enter(owner);
    }
}
