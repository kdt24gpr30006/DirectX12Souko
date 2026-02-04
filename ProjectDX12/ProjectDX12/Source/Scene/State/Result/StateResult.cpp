#include "StateResult.h"
#include "../../StateMachine/SceneStateMachine.h"
#include "../StageSelect/StateStageSelect.h"
#include "../Title/StateTitle.h"
#include "../External/Plugin/ImGui/imgui.h"
#include "../FrameWork/System/Input/Input.h"

StateResult::StateResult(int clearedStageNumber)
    : clearedStageNumber(clearedStageNumber)
{
}

StateResult::~StateResult()
{
    Exit();
}

void StateResult::Init()
{
    // マウスロック解除（ゲームシーンから遷移した場合用）
    System::Input::GetInstance()->Mouse().SetLocked(false);

    selectedIndex = 0;
}

void StateResult::Update(float dt)
{
    System::Input* input = System::Input::GetInstance();

    // 矢印キーで選択
    if (input->Keyboard().IsPush(VK_UP) || input->Keyboard().IsPush('W'))
    {
        selectedIndex = (selectedIndex - 1 + MENU_COUNT) % MENU_COUNT;
    }
    if (input->Keyboard().IsPush(VK_DOWN) || input->Keyboard().IsPush('S'))
    {
        selectedIndex = (selectedIndex + 1) % MENU_COUNT;
    }

    // EnterまたはEキーで決定
    if (input->Keyboard().IsPush(VK_RETURN) || input->Keyboard().IsPush('E'))
    {
        if (selectedIndex == 0)
        {
            stateMachine->ChangeState(new StateStageSelect());
        }
        else
        {
            stateMachine->ChangeState(new StateTitle());
        }
    }
}

void StateResult::Draw(float dt)
{
    // ImGuiでリザルトUI表示
    ImGui::SetNextWindowPos(ImVec2(350, 150), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);

    if (ImGui::Begin("Result", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar))
    {
        // クリア文字（大きく黄色で表示）
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("    STAGE CLEAR!");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Spacing();

        // ステージ番号表示
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("      Stage %d Completed!", clearedStageNumber);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Spacing();

        // メニュー選択肢
        const char* menuItems[] = {
            "Stage Select",
            "Title"
        };

        for (int i = 0; i < MENU_COUNT; ++i)
        {
            bool isSelected = (i == selectedIndex);

            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("      > %s", menuItems[i]);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::Text("        %s", menuItems[i]);
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("    Press Enter or E to Select");
    }
    ImGui::End();
}

void StateResult::Exit()
{
}
