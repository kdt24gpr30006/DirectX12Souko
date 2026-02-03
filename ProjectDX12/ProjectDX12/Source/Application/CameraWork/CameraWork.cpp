#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "../Source/Entity/Player/Player.h"
#include "System/Input/Input.h"
#include "Math/Math.h"
#include <cmath>

CameraWork::CameraWork()
    : camera(nullptr)
    , target(nullptr)
    , yaw(0.0f)
    , pitch(0.3f)  // 少し見下ろす
    , distance(10.0f)
    , height(2.0f)
    , mouseSensitivity(0.003f)
{
}

CameraWork::~CameraWork()
{
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

#include "../External/Plugin/ImGui/imgui.h"
#include <Math/Vector3/Vector3.h>

void CameraWork::DebugImGui()
{
    if (ImGui::Begin("CameraWork Debug"))
    {
        ImGui::Text("Has Camera : %s", camera ? "Yes" : "No");
        ImGui::Text("Has Target : %s", target ? "Yes" : "No");

        ImGui::SliderFloat("Distance", &distance, 2.0f, 20.0f);
        ImGui::SliderFloat("Height", &height, 0.0f, 15.0f);
        ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.001f, 0.01f);

        ImGui::SliderAngle("Yaw", &yaw);
        ImGui::SliderAngle("Pitch", &pitch);

        auto& mouse = System::Input::GetInstance()->Mouse();
        ImGui::Text("Mouse Locked: %s", mouse.IsLocked() ? "Yes" : "No");
        ImGui::Text("Mouse Delta: %d, %d", mouse.GetDeltaX(), mouse.GetDeltaY());

        ImGui::Text("CameraPos : %.2f %.2f %.2f",
            camera->GetPosition().x,
            camera->GetPosition().y,
            camera->GetPosition().z);

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

    Math::Vector3 camPos = playerPos + camOffset;

    // 注視点：プレイヤーの少し上
    Math::Vector3 lookAt = playerPos + Math::Vector3{ 0.0f, 1.5f, 0.0f };

    camera->Update(
        camPos,
        lookAt,
        Math::Vector3::Up
    );

    DebugImGui();
}
