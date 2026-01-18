#pragma once

class SceneStateMachine;

class SceneStateBase
{
public:
    virtual ~SceneStateBase() {}

    virtual void Enter() {}
    virtual void Update(float dt) = 0;
    virtual void Render(float dt) = 0;
    virtual void Exit() {}

    void SetStateMachine(SceneStateMachine* sm) { stateMachine = sm; }

protected:
    SceneStateMachine* stateMachine = nullptr;
};
