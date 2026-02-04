#pragma once

class SceneStateBase;

class SceneStateMachine
{
public:
    SceneStateMachine() = default;
    ~SceneStateMachine()
    {
        Exit();
    }

    void Init(SceneStateBase* initState);
    void Update(float dt);
    void Draw(float dt);
    void Exit();
    void ChangeState(SceneStateBase* next);

private:
    SceneStateBase* current = nullptr;
};
