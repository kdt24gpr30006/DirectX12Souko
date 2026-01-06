#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "../Source/Entity/Player/Player.h"
#include "Math/Math.h"

CameraWork::CameraWork()
    : camera(nullptr)
    , target(nullptr)
    , angle(0.0f)
    , distance(8.0f)
    , height(1.5f)
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

void CameraWork::DebugImGui()
{
    if (ImGui::Begin("CameraWork Debug"))
    {
        ImGui::Text("Has Camera : %s", camera ? "Yes" : "No");
        ImGui::Text("Has Target : %s", target ? "Yes" : "No");

        ImGui::SliderFloat("Distance", &distance, 2.0f, 20.0f);
        ImGui::SliderFloat("Height", &height, 0.0f, 10.0f);

        ImGui::SliderAngle("Yaw", &angle);

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
    {
        return;
    }

    const Math::Vector3 playerPos = target->GetPosition();
    const Math::Vector3 forward = target->GetForward().Normalized();

    Math::Vector3 camPos =
        playerPos
        - forward * distance
        + Math::Vector3::Up * height;

    camera->Update(
        camPos,
        playerPos,
        Math::Vector3::Up
    );

    DebugImGui();
}
