#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "../Source/Entity/Player/Player.h"
#include "System/Input/Input.h"
#include <cmath>
#include <Windows.h>

CameraWork::CameraWork()
    : camera(nullptr)
    , target(nullptr)
    , yaw(0.0f)
    , pitch(0.3f)  // 少し見下ろす
    , distance(10.0f)
    , height(2.0f)
    , mouseSensitivity(0.003f)
    , currentMode(CameraMode::ThirdPerson)
    , targetMode(CameraMode::ThirdPerson)
    , isTransitioning(false)
    , transitionProgress(0.0f)
    , transitionDuration(TRANSITION_DURATION)
    , transitionStartPos()
    , transitionStartLookAt()
{
}

CameraWork::~CameraWork()
{
    // マウスロック解除
    System::Input::GetInstance()->Mouse().SetLocked(false);
}

void CameraWork::Init(Camera* cam)
{
    camera = cam;

    // マウスをロック
    System::Input::GetInstance()->Mouse().SetLocked(true);
}

void CameraWork::SetTarget(const Player* player)
{
    target = player;
}

void CameraWork::AddYaw(float delta)
{
    yaw += delta;
}

void CameraWork::AddPitch(float delta)
{
    pitch += delta;
    // 垂直角度を制限
    constexpr float maxPitch = 1.4f;  // 約80度
    constexpr float minPitch = -0.5f; // 約-30度
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < minPitch) pitch = minPitch;
}

Math::Vector3 CameraWork::GetForwardXZ() const
{
    return {
        std::sin(yaw),
        0.0f,
        std::cos(yaw)
    };
}

Math::Vector3 CameraWork::GetRightXZ() const
{
    return {
        std::cos(yaw),
        0.0f,
        -std::sin(yaw)
    };
}

void CameraWork::ToggleCameraMode()
{
    // 遷移中は切り替え無視
    if (isTransitioning)
        return;

    // 現在のカメラ位置と注視点を保存
    transitionStartPos = camera->GetPosition();
    // 現在のモードに基づいて注視点を計算
    if (currentMode == CameraMode::ThirdPerson)
    {
        transitionStartLookAt = CalculateThirdPersonLookAt();
    }
    else
    {
        transitionStartLookAt = CalculateTopDownLookAt();
    }

    // モード切り替え
    if (currentMode == CameraMode::ThirdPerson)
    {
        targetMode = CameraMode::TopDown;
        // トップダウンに切り替え時はマウスロック解除
        System::Input::GetInstance()->Mouse().SetLocked(false);
    }
    else
    {
        targetMode = CameraMode::ThirdPerson;
        // 3人称に復帰時はマウスロック有効化
        System::Input::GetInstance()->Mouse().SetLocked(true);
    }

    isTransitioning = true;
    transitionProgress = 0.0f;
}

Math::Vector3 CameraWork::GetStageCenterPosition() const
{
    // ステージ中央座標: 9x9グリッド、セルサイズ10.0f -> 中央は(45, 0, 45)
    constexpr float stageSize = 9 * 10.0f; // GRID_SIZE * CELL_SIZE
    constexpr float center = stageSize / 2.0f;
    return { center, 0.0f, center };
}

Math::Vector3 CameraWork::CalculateThirdPersonPosition() const
{
    if (!target)
        return {};

    const Math::Vector3 playerPos = target->GetPosition();

    // カメラ方向を計算（球座標）
    const float sinY = std::sin(yaw);
    const float cosY = std::cos(yaw);
    const float sinP = std::sin(pitch);
    const float cosP = std::cos(pitch);

    // カメラ位置：プレイヤーの後ろ上方
    Math::Vector3 camOffset =
    {
        -sinY * cosP * distance,
        sinP * distance + height,
        -cosY * cosP * distance
    };

    return playerPos + camOffset;
}

Math::Vector3 CameraWork::CalculateThirdPersonLookAt() const
{
    if (!target)
        return {};

    const Math::Vector3 playerPos = target->GetPosition();
    // 注視点：プレイヤーの頭辺り
    return playerPos + Math::Vector3{ 0.0f, 2.5f, 0.0f };
}

Math::Vector3 CameraWork::CalculateTopDownPosition() const
{
    Math::Vector3 center = GetStageCenterPosition();
    // カメラ位置: ステージ中央の上、わずかにZ方向にオフセット（真下を見下ろす角度調整）
    return { center.x, TOP_DOWN_HEIGHT, center.z - 0.1f };
}

Math::Vector3 CameraWork::CalculateTopDownLookAt() const
{
    return GetStageCenterPosition();
}

float CameraWork::SmoothStep(float t)
{
    // イージング関数: スムーズなアニメーション
    return t * t * (3.0f - 2.0f * t);
}

void CameraWork::UpdateThirdPerson(float dt)
{
    // マウス入力でカメラ回転
    auto& mouse = System::Input::GetInstance()->Mouse();
    if (mouse.IsLocked())
    {
        AddYaw(mouse.GetDeltaX() * mouseSensitivity);
        AddPitch(mouse.GetDeltaY() * mouseSensitivity);
    }

    // マウスホイールでカメラ距離変更
    int wheelDelta = mouse.GetWheelDelta();
    if (wheelDelta != 0)
    {
        distance -= wheelDelta * 0.01f;
        // 距離の制限
        if (distance < 3.0f) distance = 3.0f;
        if (distance > 30.0f) distance = 30.0f;
    }

    // ESCでマウスロック解除/再ロック
    if (System::Input::GetInstance()->Keyboard().IsPush(VK_ESCAPE))
    {
        mouse.SetLocked(!mouse.IsLocked());
    }

    Math::Vector3 camPos = CalculateThirdPersonPosition();
    Math::Vector3 lookAt = CalculateThirdPersonLookAt();

    camera->Update(camPos, lookAt, Math::Vector3::Up);
}

void CameraWork::UpdateTopDown(float dt)
{
    // トップダウン視点ではマウス操作無効（固定視点）
    Math::Vector3 camPos = CalculateTopDownPosition();
    Math::Vector3 lookAt = CalculateTopDownLookAt();

    camera->Update(camPos, lookAt, Math::Vector3::Up);
}

void CameraWork::UpdateTransition(float dt)
{
    // 遷移進捗を更新
    transitionProgress += dt / transitionDuration;

    if (transitionProgress >= 1.0f)
    {
        // 遷移完了
        transitionProgress = 1.0f;
        isTransitioning = false;
        currentMode = targetMode;
    }

    // 目標位置・注視点を計算
    Math::Vector3 targetPos;
    Math::Vector3 targetLookAt;

    if (targetMode == CameraMode::TopDown)
    {
        targetPos = CalculateTopDownPosition();
        targetLookAt = CalculateTopDownLookAt();
    }
    else
    {
        targetPos = CalculateThirdPersonPosition();
        targetLookAt = CalculateThirdPersonLookAt();
    }

    // SmoothStepでイージング
    float t = SmoothStep(transitionProgress);

    // 補間
    Math::Vector3 currentPos = Math::Vector3::Lerp(transitionStartPos, targetPos, t);
    Math::Vector3 currentLookAt = Math::Vector3::Lerp(transitionStartLookAt, targetLookAt, t);

    camera->Update(currentPos, currentLookAt, Math::Vector3::Up);
}

#include "../External/Plugin/ImGui/imgui.h"
#include <Math/Vector3/Vector3.h>

void CameraWork::DebugImGui()
{
    if (ImGui::Begin("CameraWork Debug"))
    {
        ImGui::Text("Has Camera : %s", camera ? "Yes" : "No");
        ImGui::Text("Has Target : %s", target ? "Yes" : "No");

        // カメラモード情報
        const char* modeStr = (currentMode == CameraMode::ThirdPerson) ? "ThirdPerson" : "TopDown";
        ImGui::Text("Camera Mode: %s", modeStr);
        ImGui::Text("Transitioning: %s", isTransitioning ? "Yes" : "No");
        if (isTransitioning)
        {
            ImGui::ProgressBar(transitionProgress, ImVec2(-1, 0), "Transition");
        }

        ImGui::Separator();

        ImGui::SliderFloat("Distance", &distance, 2.0f, 20.0f);
        ImGui::SliderFloat("Height", &height, 0.0f, 15.0f);
        ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.001f, 0.01f);

        ImGui::SliderAngle("Yaw", &yaw);
        ImGui::SliderAngle("Pitch", &pitch);

        auto& mouse = System::Input::GetInstance()->Mouse();
        ImGui::Text("Mouse Locked: %s", mouse.IsLocked() ? "Yes" : "No");
        ImGui::Text("Mouse Delta: %d, %d", mouse.GetDeltaX(), mouse.GetDeltaY());

        if (camera)
        {
            ImGui::Text("CameraPos : %.2f %.2f %.2f",
                camera->GetPosition().x,
                camera->GetPosition().y,
                camera->GetPosition().z);
        }
        else
        {
            ImGui::Text("CameraPos : (null)");
        }

        if (target)
        {
            const auto& p = target->GetPosition();
            const auto& f = target->GetForward();

            ImGui::Separator();
            ImGui::Text("Player Pos : (%.2f, %.2f, %.2f)", p.x, p.y, p.z);
            ImGui::Text("Player Fwd : (%.2f, %.2f, %.2f)", f.x, f.y, f.z);
        }
    }
    ImGui::End();
}

void CameraWork::Update(float dt)
{
    if (!camera || !target)
        return;

    // Tabキーでカメラモード切り替え
    if (System::Input::GetInstance()->Keyboard().IsPush(VK_TAB))
    {
        ToggleCameraMode();
    }

    // モードに応じた更新
    if (isTransitioning)
    {
        UpdateTransition(dt);
    }
    else if (currentMode == CameraMode::ThirdPerson)
    {
        UpdateThirdPerson(dt);
    }
    else
    {
        UpdateTopDown(dt);
    }

    DebugImGui();
}
