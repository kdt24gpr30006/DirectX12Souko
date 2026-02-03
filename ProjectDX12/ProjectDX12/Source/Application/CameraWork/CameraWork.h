#pragma once
#include "Math/Vector3/Vector3.h"

class Camera;
class Player;

class CameraWork
{
public:
    CameraWork();
    ~CameraWork();

    void Init(Camera* camera);
    void Update(float dt);

    void SetTarget(const Player* player);
    void AddYaw(float delta);
    void AddPitch(float delta);

    // カメラの水平方向（Y軸回転角度）
    float GetYaw() const { return yaw; }

    // カメラの前方ベクトル（Y=0の水平面上）
    Math::Vector3 GetForwardXZ() const;

    // カメラの右方ベクトル（Y=0の水平面上）
    Math::Vector3 GetRightXZ() const;

private:
    Camera* camera;
    const Player* target;

    float yaw;      // 水平回転（Y軸周り）
    float pitch;    // 垂直回転（X軸周り）
    float distance;
    float height;
    float mouseSensitivity;

    void DebugImGui();
};
