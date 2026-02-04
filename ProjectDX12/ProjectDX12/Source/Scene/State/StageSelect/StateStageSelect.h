#pragma once
#include "../SceneStateBase.h"

class StateStageSelect : public SceneStateBase
{
public:
    StateStageSelect();
    ~StateStageSelect();

    void Init() override;
    void Update(float dt) override;
    void Draw(float dt) override;
    void Exit() override;

private:
    int selectedStageIndex = 0;  // 0-2
    static constexpr int STAGE_COUNT = 3;
};
