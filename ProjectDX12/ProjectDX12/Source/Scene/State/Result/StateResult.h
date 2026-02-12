#pragma once
#include "../SceneStateBase.h"

class Sprite;
class Texture;

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
    int selectedIndex = 0;  // 0: Title, 1: StageSelect
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
        { 170, 465, 615, 600 },   // タイトルに戻る
        { 665, 465, 1111, 600 }   // ステージセレクトに戻る
    };

    // 矢印の表示位置（各メニューのヒットボックス中央上部）
    static constexpr float arrowPositionsX[MENU_COUNT] = { 50.0f, 310.0f };
    static constexpr float arrowPositionsY[MENU_COUNT] = { 260.0f, 260.0f };

    // マウスクリック検出用
    bool prevLeftDown = false;

    // 矢印アニメーション用
    float animTime = 0.0f;
};
