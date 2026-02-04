#include "StateStageSelect.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../Game/StateGame.h"
#include "../External/Plugin/ImGui/imgui.h"
#include "../FrameWork/System/Input/Input.h"
#include <Windows.h>

StateStageSelect::StateStageSelect()
{
}

StateStageSelect::~StateStageSelect()
{
    Exit();
}

void StateStageSelect::Init()
{
    // マウスロック解除（ゲームシーンから戻った場合用）
    System::Input::GetInstance()->Mouse().SetLocked(false);

    selectedStageIndex = 0;
}

void StateStageSelect::Update(float dt)
{
    System::Input* input = System::Input::GetInstance();

    // 矢印キーで選択
    if (input->Keyboard().IsPush(VK_UP) || input->Keyboard().IsPush('W'))
    {
        selectedStageIndex = (selectedStageIndex - 1 + STAGE_COUNT) % STAGE_COUNT;
    }
    if (input->Keyboard().IsPush(VK_DOWN) || input->Keyboard().IsPush('S'))
    {
        selectedStageIndex = (selectedStageIndex + 1) % STAGE_COUNT;
    }

    // 数字キーで直接選択
    if (input->Keyboard().IsPush('1'))
    {
        selectedStageIndex = 0;
    }
    if (input->Keyboard().IsPush('2'))
    {
        selectedStageIndex = 1;
    }
    if (input->Keyboard().IsPush('3'))
    {
        selectedStageIndex = 2;
    }

    // EnterまたはEキーで決定
    if (input->Keyboard().IsPush(VK_RETURN) || input->Keyboard().IsPush('E'))
    {
        stateMachine->ChangeState(new StateGame(selectedStageIndex + 1));
    }
}

void StateStageSelect::Draw(float dt)
{
    // ImGuiでステージ選択UI表示
    ImGui::SetNextWindowPos(ImVec2(400, 200), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);

    if (ImGui::Begin("Stage Select", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Select Stage (1-3 or Arrow Keys)");
        ImGui::Separator();
        ImGui::Spacing();

        const char* stageNames[] = {
            "Stage 1 - Tutorial",
            "Stage 2 - Normal",
            "Stage 3 - Advanced"
        };

        for (int i = 0; i < STAGE_COUNT; ++i)
        {
            bool isSelected = (i == selectedStageIndex);

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("> %s", stageNames[i]);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::Text("  %s", stageNames[i]);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Press Enter or E to Start");
    }
    ImGui::End();
}

void StateStageSelect::Exit()
{
}
