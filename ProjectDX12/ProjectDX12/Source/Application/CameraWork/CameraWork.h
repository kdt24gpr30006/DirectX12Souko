#pragma once
#include "Math/Vector3/Vector3.h"

class Camera;
class Player;
class Stage;

// カメラモード列挙型
enum class CameraMode { ThirdPerson, TopDown };

class CameraWork
{
public:
    CameraWork();
    ~CameraWork();

    void Init(Camera* camera);
    void Update(float dt);

    void SetTarget(const Player* player);
    void SetStage(const Stage* stg);
    void AddYaw(float delta);
    void AddPitch(float delta);

    // カメラの水平方向（Y軸回転角度）
    float GetYaw() const { return yaw; }

    // カメラの前方ベクトル（Y=0の水平面上）
    Math::Vector3 GetForwardXZ() const;

    // カメラの右方ベクトル（Y=0の水平面上）
    Math::Vector3 GetRightXZ() const;

    // カメラモード切り替え
    void ToggleCameraMode();
    CameraMode GetCameraMode() const { return currentMode; }

    // カメラを初期状態にリセット
    void Reset();

    // 初期カメラモードを設定（遷移なしで即座に切り替え）
    void SetInitialMode(CameraMode mode);

private:
    Camera* camera;
    const Player* target;
    const Stage* stage;

    float yaw;      // 水平回転（Y軸周り）
    float pitch;    // 垂直回転（X軸周り）
    float distance;
    float height;
    float mouseSensitivity;

    // カメラモード関連
    CameraMode currentMode;                // 現在のモード
    CameraMode targetMode;                 // 遷移先モード
    bool isTransitioning;                  // 遷移中フラグ
    float transitionProgress;              // 遷移進捗 (0.0~1.0)
    float transitionDuration;              // 遷移時間（秒）
    Math::Vector3 transitionStartPos;      // 遷移開始位置
    Math::Vector3 transitionStartLookAt;   // 遷移開始注視点

    // 定数
    static constexpr float TOP_DOWN_HEIGHT = 80.0f;         // トップダウン時の高さ
    static constexpr float TRANSITION_DURATION = 0.8f;      // 遷移時間（秒）
    static constexpr float CAMERA_COLLISION_OFFSET = 0.5f;  // 壁面からのオフセット
    static constexpr float CAMERA_MIN_DISTANCE = 1.0f;      // カメラ最小距離

    // 内部メソッド
    void UpdateThirdPerson(float dt);
    void UpdateTopDown(float dt);
    void UpdateTransition(float dt);
    Math::Vector3 CalculateThirdPersonPosition() const;
    Math::Vector3 CalculateThirdPersonLookAt() const;
    Math::Vector3 CalculateTopDownPosition() const;
    Math::Vector3 CalculateTopDownLookAt() const;
    Math::Vector3 GetStageCenterPosition() const;
    static float SmoothStep(float t);

    Math::Vector3 AdjustCameraForCollision(const Math::Vector3& lookAt, const Math::Vector3& camPos) const;
    void DebugImGui();
};
