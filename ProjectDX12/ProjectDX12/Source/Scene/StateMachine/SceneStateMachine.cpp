#include "SceneStateMachine.h"
#include "../State/SceneStateBase.h"


void SceneStateMachine::Init(SceneStateBase* initState)
{
    ChangeState(initState);
}

void SceneStateMachine::Update(float dt)
{
    if (current)
        current->Update(dt);
}

void SceneStateMachine::Draw(float dt)
{
	if (current)
		current->Draw(dt);
}

void SceneStateMachine::Exit()
{
	if (current)
	{
		current->Exit();
		delete current;
		current = nullptr;
	}
}

void SceneStateMachine::ChangeState(SceneStateBase* next)
{
    // nullptrチェック
    if (!next) return;

    // 現在のStateの終了処理
    if (current)
    {
        current->Exit();
        delete current;
    }

    // 次のStateを設定
    current = next;
    if (current)
    {
        current->SetStateMachine(this);
        current->Init();
    }
}
