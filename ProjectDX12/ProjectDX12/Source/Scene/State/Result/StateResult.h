#pragma once
#include "../SceneStateBase.h"

class StateResult : public SceneStateBase
{
public:
    StateResult(int clearedStageNumber);
    ~StateResult();

    void Init() override;
    void Update(float dt) override;
    void Draw(float dt) override;
    void Exit() override;

private:
    int clearedStageNumber = 1;
    int selectedIndex = 0;  // 0: StageSelect, 1: Title
    static constexpr int MENU_COUNT = 2;
};
