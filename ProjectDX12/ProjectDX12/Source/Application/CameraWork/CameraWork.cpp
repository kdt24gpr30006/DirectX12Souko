#include "CameraWork.h"
#include "System/Camera/Camera.h"
#include "../Source/Entity/Player/Player.h"
#include "System/Input/Input.h"
#include "../Source/Stage/Stage.h"
#include "../Source/Entity/Block/Block.h"
#include <cmath>
#include <Windows.h>
#include <System/Collider/AABB/AABBCollider.h>

CameraWork::CameraWork()
    : camera(nullptr)
    , target(nullptr)
    , stage(nullptr)
    , yaw(0.0f)
    , pitch(0.5f) 
    , distance(6.0f)
    , height(7.0f)
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

void CameraWork::SetStage(const Stage* stg)
{
    stage = stg;
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

void CameraWork::Reset()
{
    yaw = 0.0f;
    pitch = 0.3f;

    currentMode = CameraMode::ThirdPerson;
    targetMode = CameraMode::ThirdPerson;
    isTransitioning = false;
    transitionProgress = 0.0f;

    System::Input::GetInstance()->Mouse().SetLocked(true);
}

void CameraWork::SetInitialMode(CameraMode mode)
{
    currentMode = mode;
    targetMode = mode;
    isTransitioning = false;
    transitionProgress = 0.0f;

    if (mode == CameraMode::TopDown)
    {
        System::Input::GetInstance()->Mouse().SetLocked(false);
    }
    else
    {
        System::Input::GetInstance()->Mouse().SetLocked(true);
    }
}

Math::Vector3 CameraWork::GetStageCenterPosition() const
{
    if (!stage)
        return {};

    const float stageSizeX = stage->GetGridWidth() * Stage::GetCellSize();
    const float stageSizeZ = stage->GetGridHeight() * Stage::GetCellSize();
    return { stageSizeX / 2.0f, 0.0f, stageSizeZ / 2.0f };
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
    // 注視点：プレイヤーの頭上
    return playerPos + Math::Vector3{ 0.0f, 5.0f, 0.0f };
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

    // ESCでマウスロック解除/再ロック
    if (System::Input::GetInstance()->Keyboard().IsPush(VK_ESCAPE))
    {
        mouse.SetLocked(!mouse.IsLocked());
    }

    Math::Vector3 camPos = CalculateThirdPersonPosition();
    Math::Vector3 lookAt = CalculateThirdPersonLookAt();

    // カメラの壁・ブロックめり込み防止
    camPos = AdjustCameraForCollision(lookAt, camPos);

    camera->Update(camPos, lookAt, Math::Vector3::Up);
}

Math::Vector3 CameraWork::AdjustCameraForCollision(const Math::Vector3& lookAt, const Math::Vector3& camPos) const
{
    if (!stage)
        return camPos;

    Math::Vector3 diff = camPos - lookAt;
    float rayLength = Math::Vector3::Length(diff);

    if (rayLength < 1e-4f)
        return camPos;

    Math::Vector3 dir = diff / rayLength;
    float closestHit = rayLength;
    float hitDist = 0.0f;

    // 壁コライダーとの判定
    const auto& walls = stage->GetWallColliders();
    for (const auto& wall : walls)
    {
        if (wall.CastRay(lookAt, dir, rayLength, hitDist))
        {
            if (hitDist < closestHit)
                closestHit = hitDist;
        }
    }

    // ブロックコライダーとの判定（カメラ用に高さを拡張）
    constexpr float BLOCK_CAM_HEIGHT = 50.0f;
    const auto& blocks = stage->GetBlocks();
    for (const auto& block : blocks)
    {
        const AABBCollider* col = block->GetCollider();
        if (!col) continue;

        AABBCollider::Range xr = col->GetAxisXRange();
        AABBCollider::Range zr = col->GetAxisZRange();
        AABBCollider tallCol;
        float cx = (xr.Min + xr.Max) * 0.5f;
        float cz = (zr.Min + zr.Max) * 0.5f;
        tallCol.SetCenter(Math::Vector3(cx, BLOCK_CAM_HEIGHT * 0.5f, cz));
        tallCol.SetVolume(Math::Vector3(xr.Max - xr.Min, BLOCK_CAM_HEIGHT, zr.Max - zr.Min));

        if (tallCol.CastRay(lookAt, dir, rayLength, hitDist))
        {
            if (hitDist < closestHit)
                closestHit = hitDist;
        }
    }

    // ヒットがあればカメラ位置を調整
    if (closestHit < rayLength)
    {
        float adjustedDist = closestHit - CAMERA_COLLISION_OFFSET;
        if (adjustedDist < CAMERA_MIN_DISTANCE)
            adjustedDist = CAMERA_MIN_DISTANCE;
        return lookAt + dir * adjustedDist;
    }

    return camPos;
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

#ifdef _DEBUG
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
#else
void CameraWork::DebugImGui() {}
#endif

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
