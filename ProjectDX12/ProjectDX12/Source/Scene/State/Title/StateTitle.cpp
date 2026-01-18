#include "StateTitle.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Game/StateGame.h"
#include "../FrameWork/Graphics/Sprite/Sprite.h"
#include "../FrameWork/Graphics/Resource/TextureManager.h"
#include "../FrameWork/System/Input/Input.h"
#include "../FrameWork/Math/Vector2/Vector2.h"

StateTitle::StateTitle()
{
}

StateTitle::~StateTitle()
{
    Exit();
}

void StateTitle::Init()
{
    // Sprite 作成
    titleSprite = new Sprite();
    titleSprite->Create();

    // Texture を ResourceManager から取得
    Texture* tex =
        TextureManager::Instance().LoadTexture(
            "Assets/TitleImage.dds"
        );

    assert(tex && "TitleImage.dds の読み込みに失敗しています");

    titleSprite->SetTexture(tex);

    // 画面中央表示用設定
    titleSprite->SetPivot(Math::Vector2(0.5f, 0.5f));
    titleSprite->SetPosition(Math::Vector2(0.0f, 0.0f));
    titleSprite->SetScale(Math::Vector2(1.0f, 1.0f));
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

    // Enter キーでゲーム開始
    if (input->Keyboard().IsPush(VK_RETURN))
    {
        stateMachine->ChangeState(new StateGame());
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
