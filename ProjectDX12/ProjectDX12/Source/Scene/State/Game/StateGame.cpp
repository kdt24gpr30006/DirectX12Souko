#include "StateGame.h"
#include <new>
#include "../FrameWork/System/Camera/Camera.h"
#include "../FrameWork/System/Input/Input.h"
#include "../Source/Entity/Player/Player.h"
#include "../Source/Stage/Stage.h"
#include "../Source/Application/CameraWork/CameraWork.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Result/StateResult.h"
#include "../GameOver/StateGameOver.h"
#include "../StageSelect/StateStageSelect.h"
#include "../FrameWork/Graphics/Sprite/Sprite.h"
#include "../FrameWork/Graphics/Resource/TextureManager.h"
#include "../FrameWork/Math/Vector2/Vector2.h"
#include <Graphics/Texture/Texture.h>
#include <Windows.h>
#include <cassert>
#include <cmath>
#include <Graphics/Color/Color.h>



StateGame::StateGame(int stageNumber)
    : currentStageNumber(stageNumber)
{
}

StateGame::~StateGame()
{
	Exit();
}

void StateGame::Init()
{
	// Stage生成
	stage = new Stage();
	stage->Init(currentStageNumber);

	// Player生成
	player = new Player();
	player->Init(stage, stage->GetPlayerStartPos());

	// カメラ作成
	camera = new Camera();
	camera->Create();

	// CameraWork作成
	cameraWork = new CameraWork();
	cameraWork->Init(camera);
	cameraWork->SetTarget(player);
	cameraWork->SetStage(stage);

	// トップダウン視点で開始（TPS視点へ自動遷移）
	cameraWork->SetInitialMode(CameraMode::TopDown);
	introTimer = 0.0f;
	introComplete = false;

	// PlayerにCameraWorkを渡す
	player->SetCameraWork(cameraWork);

	// ポーズ画面のスプライト初期化
	isPaused = false;
	pauseSelectedIndex = 0;
	pausePrevLeftDown = false;

	Texture* pauseBgTex = TextureManager::Instance().LoadTexture("Assets/Pause.dds");
	assert(pauseBgTex && "Pause.dds の読み込みに失敗しました");

	// 暗いオーバーレイ（全画面半透明黒）— 全面不透明なテクスチャを使用
	Texture* overlayTex = TextureManager::Instance().LoadTexture("Assets/StageSelect.dds");
	assert(overlayTex && "StageSelect.dds の読み込みに失敗しました");

	pauseOverlaySprite = new Sprite();
	pauseOverlaySprite->Create();
	pauseOverlaySprite->SetTexture(overlayTex);
	pauseOverlaySprite->SetPivot(Math::Vector2(0.f, 0.f));
	pauseOverlaySprite->SetPosition(Math::Vector2(0.0f, 0.0f));
	pauseOverlaySprite->SetScale(Math::Vector2(1.0f, 1.0f));
	pauseOverlaySprite->SetSize(Math::Vector2(1280.0f, 720.0f));
	pauseOverlaySprite->SetColor(Color(0.0f, 0.0f, 0.0f, 0.5f));

	// ポーズメニュー画像（画面中央に配置）
	pauseBackgroundSprite = new Sprite();
	pauseBackgroundSprite->Create();
	pauseBackgroundSprite->SetTexture(pauseBgTex);
	pauseBackgroundSprite->SetPivot(Math::Vector2(0.f, 0.f));
	// 中央配置: Position = ((1280 - 500) / 2 / 2, (720 - 279) / 2 / 2)
	pauseBackgroundSprite->SetPosition(Math::Vector2(195.0f, 110.25f));
	pauseBackgroundSprite->SetScale(Math::Vector2(1.0f, 1.0f));
	pauseBackgroundSprite->SetSize(Math::Vector2((float)pauseBgTex->GetWidth(), (float)pauseBgTex->GetHeight()));

	// 矢印
	pauseArrowSprite = new Sprite();
	pauseArrowSprite->Create();

	Texture* pauseArrowTex = TextureManager::Instance().LoadTexture("Assets/yaji.dds");
	assert(pauseArrowTex && "yaji.dds の読み込みに失敗しました");

	pauseArrowSprite->SetTexture(pauseArrowTex);
	pauseArrowSprite->SetPivot(Math::Vector2(0.5f, 0.5f));
	pauseArrowSprite->SetAngle(270.0f);
	pauseArrowSprite->SetScale(Math::Vector2(0.1f, 0.1f));
	pauseArrowSprite->SetSize(Math::Vector2((float)pauseArrowTex->GetWidth(), (float)pauseArrowTex->GetHeight()));

	// ESCアイコン（左上に配置）
	escSprite = new Sprite();
	escSprite->Create();

	Texture* escTex = TextureManager::Instance().LoadTexture("Assets/ESC.dds");
	assert(escTex && "ESC.dds の読み込みに失敗しました");

	escSprite->SetTexture(escTex);
	escSprite->SetPivot(Math::Vector2(0.f, 0.f));
	escSprite->SetPosition(Math::Vector2(5.0f, 5.0f));
	escSprite->SetScale(Math::Vector2(0.5f, 0.5f));
	escSprite->SetSize(Math::Vector2((float)escTex->GetWidth(), (float)escTex->GetHeight()));
}

void StateGame::Update(float dt)
{
	System::Input* input = System::Input::GetInstance();

	// ESCキーでポーズ切り替え
	if (input->Keyboard().IsPush(VK_ESCAPE))
	{
		isPaused = !isPaused;
		if (isPaused)
		{
			pauseSelectedIndex = 0;
			pausePrevLeftDown = false;
			// ポーズ中はマウスロック解除
			input->Mouse().SetLocked(false);
		}
		else
		{
			// ポーズ解除時にマウスロック復帰
			input->Mouse().SetLocked(true);
		}
		return;
	}

	// ポーズ中の入力処理
	if (isPaused)
	{
		// A/Dキーまたは左右矢印キーで選択
		if (input->Keyboard().IsPush('A') || input->Keyboard().IsPush(VK_LEFT))
		{
			pauseSelectedIndex = (pauseSelectedIndex - 1 + PAUSE_MENU_COUNT) % PAUSE_MENU_COUNT;
		}
		if (input->Keyboard().IsPush('D') || input->Keyboard().IsPush(VK_RIGHT))
		{
			pauseSelectedIndex = (pauseSelectedIndex + 1) % PAUSE_MENU_COUNT;
		}

		// マウスで選択と決定
		bool leftDown = input->Mouse().IsLeftDown();
		int mouseX = input->Mouse().GetX();
		int mouseY = input->Mouse().GetY();

		// マウスカーソルがヒットボックス上にあれば選択を移動
		for (int i = 0; i < PAUSE_MENU_COUNT; ++i)
		{
			const MenuHitBox& box = pauseMenuHitBoxes[i];
			if (mouseX >= box.left && mouseX <= box.right &&
				mouseY >= box.top && mouseY <= box.bottom)
			{
				pauseSelectedIndex = i;
				break;
			}
		}

		bool clicked = (leftDown && !pausePrevLeftDown);
		pausePrevLeftDown = leftDown;

		if (clicked)
		{
			for (int i = 0; i < PAUSE_MENU_COUNT; ++i)
			{
				const MenuHitBox& box = pauseMenuHitBoxes[i];
				if (mouseX >= box.left && mouseX <= box.right &&
					mouseY >= box.top && mouseY <= box.bottom)
				{
					if (i == 0)
					{
						isPaused = false;
						ResetStage();
					}
					else
					{
						stateMachine->ChangeState(new StateStageSelect());
					}
					return;
				}
			}
		}

		// EnterまたはEキーで決定
		if (input->Keyboard().IsPush(VK_RETURN) || input->Keyboard().IsPush('E'))
		{
			if (pauseSelectedIndex == 0)
			{
				isPaused = false;
				ResetStage();
			}
			else
			{
				stateMachine->ChangeState(new StateStageSelect());
			}
			return;
		}

		pauseAnimTime += dt;
		return; // ポーズ中はゲーム更新をスキップ
	}

	// イントロ演出：3秒後にトップダウン→TPS視点へ自動遷移
	if (!introComplete)
	{
		introTimer += dt;
		if (introTimer >= INTRO_DURATION)
		{
			cameraWork->ToggleCameraMode();
			introComplete = true;
		}
	}

	player->Update(dt);

	// カメラ更新（マウス入力はCameraWork内で処理）
	cameraWork->Update(dt);

	stage->Update(dt);

	// Rキーでリセット
	if (input->Keyboard().IsPush('R'))
	{
		ResetStage();
		return;
	}

	// 爆発チェック → ゲームオーバー画面へ遷移
	if (stage->HasExplosion())
	{
		stateMachine->ChangeState(new StateGameOver(currentStageNumber));
		return;
	}

	// ゴールチェック
	if (stage->HasGoal())
	{
		stateMachine->ChangeState(new StateResult(currentStageNumber));
		return;
	}

#ifdef _DEBUG
	if (ImGui::Begin("Debug Grid"))
	{
		// Player
		Int2 p = player->GetGridPos();
		ImGui::Text("Player Grid : (%d, %d)", p.x, p.y);

		// Blocks
		int i = 0;
		for (auto& block : stage->GetBlocks())
		{
			Int2 b = block->GetGridPos();
			ImGui::Text("Block %d Grid : (%d, %d)", i, b.x, b.y);
			++i;
		}

		ImGui::Text("IsExplosion : %d", stage->HasExplosion());
		ImGui::Text("IsGoal : %d", stage->HasGoal());
	}
	ImGui::End();
#endif
}

void StateGame::Draw(float dt)
{
	stage->Draw();
	player->Draw();

	// ESCアイコン描画
	if (escSprite && !isPaused)
	{
		escSprite->Draw();
	}

	// ポーズ画面の描画
	if (isPaused)
	{
		// 半透明黒オーバーレイ
		if (pauseOverlaySprite)
		{
			pauseOverlaySprite->Draw();
		}
		// ポーズメニュー画像
		if (pauseBackgroundSprite)
		{
			pauseBackgroundSprite->Draw();
		}
		// 選択矢印
		if (pauseArrowSprite)
		{
			float waveOffset = sinf(pauseAnimTime * 3.0f) * 3.0f;
			pauseArrowSprite->SetPosition(Math::Vector2(pauseArrowPositionsX[pauseSelectedIndex] + waveOffset, pauseArrowPositionsY[pauseSelectedIndex]));
			pauseArrowSprite->Draw();
		}
	}
}

void StateGame::ResetStage()
{
	stage->Reset();
	player->ResetToStart(stage->GetPlayerStartPos());
	cameraWork->Reset();
}

void StateGame::Exit()
{
	// マウスロック解除
	System::Input::GetInstance()->Mouse().SetLocked(false);

	if (player)
	{
		delete player;
		player = nullptr;
	}

	if (stage)
	{
		delete stage;
		stage = nullptr;
	}

	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (cameraWork)
	{
		delete cameraWork;
		cameraWork = nullptr;
	}

	if (pauseOverlaySprite)
	{
		delete pauseOverlaySprite;
		pauseOverlaySprite = nullptr;
	}
	if (pauseBackgroundSprite)
	{
		delete pauseBackgroundSprite;
		pauseBackgroundSprite = nullptr;
	}
	if (pauseArrowSprite)
	{
		delete pauseArrowSprite;
		pauseArrowSprite = nullptr;
	}
	if (escSprite)
	{
		delete escSprite;
		escSprite = nullptr;
	}
}
