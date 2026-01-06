#pragma once

class Sprite;
class Field;
class Stage;
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
    Stage* stage;
    Player* player;
    CameraWork* cameraWork;
};