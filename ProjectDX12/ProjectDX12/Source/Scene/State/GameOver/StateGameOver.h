#pragma once
#include "../SceneStateBase.h"

class Sprite;
class Texture;

class StateGameOver : public SceneStateBase
{
public:
    StateGameOver(int stageNumber);
    ~StateGameOver();

    void Init() override;
    void Update(float dt) override;
    void Draw(float dt) override;
    void Exit() override;

private:
    int currentStageNumber = 1;
    int selectedIndex = 0;  // 0: Retry, 1: Title
    static constexpr int MENU_COUNT = 2;

    // 背景画像
    Sprite* backgroundSprite = nullptr;
    // 矢印画像
    Sprite* arrowSprite = nullptr;

    // マウスクリック判定用の座標
    struct MenuHitBox {
        int left, top, right, bottom;
    };
    static constexpr MenuHitBox menuHitBoxes[MENU_COUNT] = {
        { 240, 550, 520, 620 },   // リトライ
        { 760, 550, 1040, 620 }   // タイトルに戻る
    };

    // 矢印の表示位置（各メニューのヒットボックス中央上部）
    static constexpr float arrowPositionsX[MENU_COUNT] = { 60.0f, 320.0f };
    static constexpr float arrowPositionsY[MENU_COUNT] = { 310.0f, 310.0f };

    // マウスクリック検出用
    bool prevLeftDown = false;

    // 矢印アニメーション用
    float animTime = 0.0f;
};
