#include "StateGameOver.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Game/StateGame.h"
#include "../Title/StateTitle.h"
#include "../FrameWork/Graphics/Sprite/Sprite.h"
#include "../FrameWork/Graphics/Resource/TextureManager.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Vector2/Vector2.h"
#include <Graphics/Texture/Texture.h>
#include <Windows.h>
#include <cassert>

StateGameOver::StateGameOver(int stageNumber)
    : currentStageNumber(stageNumber)
{
}

StateGameOver::~StateGameOver()
{
    Exit();
}

void StateGameOver::Init()
{
    // マウスロック解除（ゲームシーンから遷移した場合用）
    System::Input::GetInstance()->Mouse().SetLocked(false);

    selectedIndex = 0;
    prevLeftDown = false;

    // 背景画像の読み込み
    backgroundSprite = new Sprite();
    backgroundSprite->Create();

    Texture* bgTex = TextureManager::Instance().LoadTexture("Assets/GameOver.dds");
    assert(bgTex && "GameOver.dds の読み込みに失敗しました");

    backgroundSprite->SetTexture(bgTex);
    backgroundSprite->SetPivot(Math::Vector2(0.f, 0.f));
    backgroundSprite->SetPosition(Math::Vector2(0.0f, 0.0f));
    backgroundSprite->SetScale(Math::Vector2(1.0f, 1.0f));
    backgroundSprite->SetSize(Math::Vector2((float)bgTex->GetWidth(), (float)bgTex->GetHeight()));

    // 矢印画像の読み込み
    arrowSprite = new Sprite();
    arrowSprite->Create();

    Texture* arrowTex = TextureManager::Instance().LoadTexture("Assets/yaji.dds");
    assert(arrowTex && "yaji.dds の読み込みに失敗しました");

    arrowSprite->SetTexture(arrowTex);
    arrowSprite->SetPivot(Math::Vector2(0.5f, 0.5f));  // 中央をピボットに
    arrowSprite->SetAngle(270.0f);
    arrowSprite->SetScale(Math::Vector2(0.3f, 0.3f));
    arrowSprite->SetSize(Math::Vector2((float)arrowTex->GetWidth(), (float)arrowTex->GetHeight()));
}

void StateGameOver::Update(float dt)
{
    System::Input* input = System::Input::GetInstance();

    // A/Dキーまたは左右矢印キーで選択
    if (input->Keyboard().IsPush('A') || input->Keyboard().IsPush(VK_LEFT))
    {
        selectedIndex = (selectedIndex - 1 + MENU_COUNT) % MENU_COUNT;
    }
    if (input->Keyboard().IsPush('D') || input->Keyboard().IsPush(VK_RIGHT))
    {
        selectedIndex = (selectedIndex + 1) % MENU_COUNT;
    }

    // マウスクリックで選択と決定
    bool leftDown = input->Mouse().IsLeftDown();
    int mouseX = input->Mouse().GetX();
    int mouseY = input->Mouse().GetY();

    // クリックした瞬間を検出
    bool clicked = (leftDown && !prevLeftDown);
    prevLeftDown = leftDown;

    if (clicked)
    {
        for (int i = 0; i < MENU_COUNT; ++i)
        {
            const MenuHitBox& box = menuHitBoxes[i];
            if (mouseX >= box.left && mouseX <= box.right &&
                mouseY >= box.top && mouseY <= box.bottom)
            {
                // クリックしたメニューに即座に遷移
                if (i == 0)
                {
                    stateMachine->ChangeState(new StateGame(currentStageNumber));
                }
                else
                {
                    stateMachine->ChangeState(new StateTitle());
                }
                return;
            }
        }
    }

    // EnterまたはEキーで決定
    if (input->Keyboard().IsPush(VK_RETURN) || input->Keyboard().IsPush('E'))
    {
        if (selectedIndex == 0)
        {
            stateMachine->ChangeState(new StateGame(currentStageNumber));
        }
        else
        {
            stateMachine->ChangeState(new StateTitle());
        }
    }
}

void StateGameOver::Draw(float dt)
{
    // 背景画像を描画
    if (backgroundSprite)
    {
        backgroundSprite->Draw();
    }

    // 矢印を選択中のメニューの位置に描画
    if (arrowSprite)
    {
        arrowSprite->SetPosition(Math::Vector2(arrowPositionsX[selectedIndex], arrowPositionsY[selectedIndex]));
        arrowSprite->Draw();
    }
}

void StateGameOver::Exit()
{
    if (backgroundSprite)
    {
        delete backgroundSprite;
        backgroundSprite = nullptr;
    }
    if (arrowSprite)
    {
        delete arrowSprite;
        arrowSprite = nullptr;
    }
}
