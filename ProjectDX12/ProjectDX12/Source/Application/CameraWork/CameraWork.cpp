#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "../Source/Entity/Player/Player.h"
#include "Math/Math.h"
#include <cmath> // 追加

CameraWork::CameraWork()
    : camera(nullptr)
    , target(nullptr)
    , angle(0.0f)
    , distance(8.0f)
    , height(8.0f)
    , rotateSpeed(60.0f * Math::RAD)
{
}

CameraWork::~CameraWork()
{
}

void CameraWork::Initialize(Camera* cam)
{
    camera = cam;
}

void CameraWork::SetTarget(const Player* player)
{
    target = player;
}

void CameraWork::AddYaw(float delta)
{
    angle += delta;
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

        ImGui::SliderAngle("Yaw", &angle);

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

    const Math::Vector3 playerPos = target->GetPosition();

    // ===== Yawからカメラ方向を作る =====
    const float sinY = std::sin(angle);
    const float cosY = std::cos(angle);

    // 左手座標系（LH）想定
    Math::Vector3 camDir =
    {
        sinY,
        0.0f,
        cosY
    };
    camDir.Normalize();

    // ===== カメラ位置 =====
    Math::Vector3 camPos =
        playerPos
        - camDir * distance
        + Math::Vector3::Up * height;

    // ===== 注視点 =====
    Math::Vector3 lookAt =
        playerPos + Math::Vector3::Up * height * 0.7f;

    camera->Update(
        camPos,
        lookAt,
        Math::Vector3::Up
    );

    DebugImGui();
}
