#include "StateStageSelect.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Game/StateGame.h"
#include "../FrameWork/Graphics/Sprite/Sprite.h"
#include "../FrameWork/Graphics/Resource/TextureManager.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Vector2/Vector2.h"
#include <Graphics/Texture/Texture.h>
#include <Windows.h>
#include <cassert>

StateStageSelect::StateStageSelect()
{
}

StateStageSelect::~StateStageSelect()
{
    Exit();
}

void StateStageSelect::Init()
{
    // マウスロック解除（ゲームシーンから戻った場合用）
    System::Input::GetInstance()->Mouse().SetLocked(false);

    selectedStageIndex = 0;
    prevLeftDown = false;

    // 背景画像の読み込み
    backgroundSprite = new Sprite();
    backgroundSprite->Create();

    Texture* bgTex = TextureManager::Instance().LoadTexture("Assets/StageSelect.dds");
    assert(bgTex && "StageSelect.dds の読み込みに失敗しました");

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
    arrowSprite->SetScale(Math::Vector2(0.3f, 0.3f));
    arrowSprite->SetSize(Math::Vector2((float)arrowTex->GetWidth(), (float)arrowTex->GetHeight()));
}

void StateStageSelect::Update(float dt)
{
    System::Input* input = System::Input::GetInstance();

    // A/Dキーまたは左右矢印キーで選択
    if (input->Keyboard().IsPush('A') || input->Keyboard().IsPush(VK_LEFT))
    {
        selectedStageIndex = (selectedStageIndex - 1 + STAGE_COUNT) % STAGE_COUNT;
    }
    if (input->Keyboard().IsPush('D') || input->Keyboard().IsPush(VK_RIGHT))
    {
        selectedStageIndex = (selectedStageIndex + 1) % STAGE_COUNT;
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
        for (int i = 0; i < STAGE_COUNT; ++i)
        {
            const StageHitBox& box = stageHitBoxes[i];
            if (mouseX >= box.left && mouseX <= box.right &&
                mouseY >= box.top && mouseY <= box.bottom)
            {
                // クリックしたステージに即座に遷移
                stateMachine->ChangeState(new StateGame(i + 1));
                return;
            }
        }
    }

    // EnterまたはEキーで決定
    if (input->Keyboard().IsPush(VK_RETURN) || input->Keyboard().IsPush('E'))
    {
        stateMachine->ChangeState(new StateGame(selectedStageIndex + 1));
    }
}

void StateStageSelect::Draw(float dt)
{
    // 背景画像を描画
    if (backgroundSprite)
    {
        backgroundSprite->Draw();
    }

    // 矢印を選択中のステージの位置に描画
    if (arrowSprite)
    {
        arrowSprite->SetPosition(Math::Vector2(arrowPositionsX[selectedStageIndex], arrowPositionsY[selectedStageIndex]));
        arrowSprite->Draw();
    }
}

void StateStageSelect::Exit()
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
