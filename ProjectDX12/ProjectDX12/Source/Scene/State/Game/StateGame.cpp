#include "StateGame.h"
#include <memory>
#include <new>
#include "../External/Plugin/ImGui/imgui.h"
#include "../FrameWork/System/Camera/Camera.h"
#include "../FrameWork/System/Input/Input.h"
#include "../Source/Entity/Player/Player.h"
#include "../Source/Stage/Stage.h"
#include "../Source/Application/CameraWork/CameraWork.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Result/StateResult.h"
#include <Math/Int2/Int2.h>



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
	player->Init(stage);

	// カメラ作成
	camera = new Camera();
	camera->Create();

	// CameraWork作成
	cameraWork = new CameraWork();
	cameraWork->Init(camera);
	cameraWork->SetTarget(player);

	// PlayerにCameraWorkを渡す
	player->SetCameraWork(cameraWork);
}

void StateGame::Update(float dt)
{
	player->Update(dt);

	// カメラ更新（マウス入力はCameraWork内で処理）
	cameraWork->Update(dt);

	stage->Update(dt);

	// 爆発チェック
	if (stage->HasExplosion())
	{
		stateMachine->ChangeState(new StateGame(currentStageNumber));
		return;
	}

	// ゴールチェック
	if (stage->HasGoal())
	{
		stateMachine->ChangeState(new StateResult(currentStageNumber));
		return;
	}

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
}

void StateGame::Draw(float dt)
{
	stage->Draw();
	player->Draw();
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
}
