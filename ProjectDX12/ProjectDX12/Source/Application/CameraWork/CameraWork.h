#pragma once
#include "Math/Vector3/Vector3.h"

class Camera;
class Player;

class CameraWork
{
public:
    CameraWork();
    ~CameraWork();

    void Initialize(Camera* camera);
    void Update(float dt);

    void SetTarget(const Player* player);
    void AddYaw(float delta);

private:
    Camera* camera;
    const Player* target;

    float angle;
    float distance;
    float height;
    float rotateSpeed;

    void DebugImGui();
};
