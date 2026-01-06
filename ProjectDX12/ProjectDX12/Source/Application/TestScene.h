#pragma once

class Sprite;
class Player;
class Stage;
class Camera;
class CameraWork;

class TestScene
{
public:
	TestScene();
	~TestScene();

	bool Initialize();
	void Release();
	void Update();
	void Render();

private:

	Sprite* sprite = nullptr;
	Player* player = nullptr;
	Stage* stage = nullptr;
	Camera* camera = nullptr;
	CameraWork* cameraWork = nullptr;
};
