#pragma once
#include "../SceneStateBase.h"

class Sprite;

class StateTitle : public SceneStateBase
{
public:
    StateTitle();
    ~StateTitle();

    void Init() override;
    void Update(float dt) override;
    void Draw(float dt) override;
    void Exit() override;

private:
    Sprite* titleSprite = nullptr;
};