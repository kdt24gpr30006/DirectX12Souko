#pragma once
#include "../SceneStateBase.h"

class Sprite;
class Texture;

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

    // 背景画像
    Sprite* backgroundSprite = nullptr;
    // 矢印画像
    Sprite* arrowSprite = nullptr;

    // マウスクリック判定用の座標
    struct StageHitBox {
        int left, top, right, bottom;
    };
    static constexpr StageHitBox stageHitBoxes[STAGE_COUNT] = {
        { 90, 260, 425, 520 },   // ステージ1
        { 450, 260, 800, 520 },  // ステージ2
        { 850, 260, 1200, 520 }  // ステージ3
    };

    // 矢印の表示位置（各ステージのヒットボックス中央）
    static constexpr float arrowPositionsX[STAGE_COUNT] = { 120.0f, 315.0f, 510.0f };
    static constexpr float arrowPositionsY[STAGE_COUNT] = { 300.0f, 300.0f, 300.0f };

    // マウスクリック検出用
    bool prevLeftDown = false;

    // 矢印アニメーション用
    float animTime = 0.0f;
};
