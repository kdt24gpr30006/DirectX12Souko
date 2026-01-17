#pragma once

class SceneStateBase;

class SceneStateMachine
{
public:
    ~SceneStateMachine();

    void Initialize(SceneStateBase* initState);
    void Update(float dt);
    void Render(float dt);
    void Exit();
    void ChangeState(SceneStateBase* next);

private:
    SceneStateBase* current = nullptr;
};
