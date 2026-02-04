#include "StateTitle.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../StageSelect/StateStageSelect.h"
#include "../FrameWork/Graphics/Sprite/Sprite.h"
#include "../FrameWork/Graphics/Resource/TextureManager.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Vector2/Vector2.h"
#include <Graphics/Texture/Texture.h>
#include <cassert>

StateTitle::StateTitle()
{
}

StateTitle::~StateTitle()
{
    Exit();
}

void StateTitle::Init()
{
    // マウスロック解除（ゲームシーンから戻った場合用）
    System::Input::GetInstance()->Mouse().SetLocked(false);

    // Sprite 作成
    titleSprite = new Sprite();
    titleSprite->Create();

    // Texture を ResourceManager から取得
    Texture* tex =
        TextureManager::Instance().LoadTexture(
            "Assets/TitleImage.dds"
        );

    assert(tex && "TitleImage.dds の読み込みに失敗しました");

    titleSprite->SetTexture(tex);

    // 画面中央に表示するための設定
    titleSprite->SetPivot(Math::Vector2(0.f, 0.f));
    titleSprite->SetPosition(Math::Vector2(0.0f, 0.0f));
    titleSprite->SetScale(Math::Vector2(0.3f, 0.3f));
    // サイズはテクスチャそのまま
    titleSprite->SetSize(
        Math::Vector2(
            (float)tex->GetWidth(),
            (float)tex->GetHeight()
        )
    );
}

void StateTitle::Update(float dt)
{
    System::Input* input = System::Input::GetInstance();

    // Enter キーでステージセレクトへ遷移
    if (input->Keyboard().IsPush('E'))
    {
        stateMachine->ChangeState(new StateStageSelect());
    }
}

void StateTitle::Draw(float dt)
{
    if (titleSprite)
    {
        titleSprite->Draw();
    }
}

void StateTitle::Exit()
{
    if (titleSprite)
    {
        delete titleSprite;
        titleSprite = nullptr;
    }
}
