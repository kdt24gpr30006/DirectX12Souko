#pragma once

class FbxMesh;
class Sprite;
class Field;
class Player;
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

	Sprite* sprite;

	Field* field;
	Player* player;
	CameraWork* cameraWork;
};

