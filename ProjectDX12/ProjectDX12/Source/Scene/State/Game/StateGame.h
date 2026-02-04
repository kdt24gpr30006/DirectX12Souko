#pragma once
#include "../SceneStateBase.h"

class Player;
class Stage;
class Camera;
class CameraWork;
class Field;


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

	Player* player = nullptr;
	Stage* stage = nullptr;
	Camera* camera = nullptr;
	CameraWork* cameraWork = nullptr;
	Field* field = nullptr;
};