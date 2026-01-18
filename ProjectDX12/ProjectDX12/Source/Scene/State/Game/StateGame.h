#pragma once
#include "../SceneStateBase.h"

class Sprite;
class Player;
class Stage;
class Camera;
class CameraWork;
class Field;

class StateGame : public SceneStateBase
{
public:
	StateGame();
	~StateGame();

	void Init();
	void Update(float dt);
	void Draw(float dt);
	void Exit();

private:
	Sprite* sprite = nullptr;
	Player* player = nullptr;
	Stage* stage = nullptr;
	Camera* camera = nullptr;
	CameraWork* cameraWork = nullptr;
	Field* field = nullptr;
};