#pragma once
#include "../SceneStateBase.h"

class Player;
class Stage;
class Camera;
class CameraWork;
class Field;
class Sprite;


class StateGame : public SceneStateBase
{
public:
	StateGame(int stageNumber = 1);
	~StateGame();

	void Init();
	void Update(float dt);
	void Draw(float dt);
	void Exit();

private:

	// 現在のステージ番号
	int currentStageNumber = 1;

	/// <summary>
	/// ステージをリセット（GPU資源を保持したまま初期状態に戻す）
	/// </summary>
	void ResetStage();

	Player* player = nullptr;
	Stage* stage = nullptr;
	Camera* camera = nullptr;
	CameraWork* cameraWork = nullptr;
	Field* field = nullptr;

	// ポーズ画面
	bool isPaused = false;
	int pauseSelectedIndex = 0;  // 0: リトライ, 1: ステージ選択
	static constexpr int PAUSE_MENU_COUNT = 2;

	// 暗いオーバーレイ（全画面半透明黒）
	Sprite* pauseOverlaySprite = nullptr;
	// ポーズメニュー画像（中央配置）
	Sprite* pauseBackgroundSprite = nullptr;
	Sprite* pauseArrowSprite = nullptr;

	// Pause.dds (500x279) を 1280x720 の中央に配置
	// 画面上の左上: (390, 220), 右下: (890, 499)
	// 画像ローカル座標 → 画面座標: screen = local + offset(390, 220)
	struct MenuHitBox {
		int left, top, right, bottom;
	};
	static constexpr MenuHitBox pauseMenuHitBoxes[PAUSE_MENU_COUNT] = {
		{ 460, 400, 570, 460 },   // リトライ     (画像内 70,180 ~ 180,240)
		{ 650, 400, 860, 460 }    // ステージ選択 (画像内 260,180 ~ 470,240)
	};

	// 矢印位置（Position空間 = 画面ピクセル / 2）
	static constexpr float pauseArrowPositionsX[PAUSE_MENU_COUNT] = { 218.0f, 313.0f };
	static constexpr float pauseArrowPositionsY[PAUSE_MENU_COUNT] = { 212.0f, 212.0f };

	bool pausePrevLeftDown = false;

	// 矢印アニメーション用
	float pauseAnimTime = 0.0f;

	// ESCアイコン（左上表示）
	Sprite* escSprite = nullptr;
};