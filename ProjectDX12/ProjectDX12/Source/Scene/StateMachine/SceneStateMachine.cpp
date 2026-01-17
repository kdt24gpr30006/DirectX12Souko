#include "SceneStateMachine.h"
#include "../State/SceneStateBase.h"

SceneStateMachine::~SceneStateMachine()
{
    delete current;
}

void SceneStateMachine::Initialize(SceneStateBase* initState)
{
    ChangeState(initState);
}

void SceneStateMachine::Update(float dt)
{
    if (current)
        current->Update(dt);
}

void SceneStateMachine::ChangeState(SceneStateBase* next)
{
    if (current)
    {
        current->Exit();
        delete current;
    }

    current = next;

    if (current)
    {
        current->SetStateMachine(this);
        current->Enter();
    }
}
