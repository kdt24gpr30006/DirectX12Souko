#include "SokobanRenderer.h"
#include "SokobanGame.h"
#include "Graphics/FbxMesh/FbxMesh.h"
#include "Math/Quaternion/Quaternion.h"
#include <cmath>

namespace Sokoban
{
    SokobanRenderer::~SokobanRenderer()
    {
        Release();
    }

    bool SokobanRenderer::Initialize(const SokobanGame* inGame)
    {
        game = inGame;
        if (!game) return false;

        // プレイヤーモデル
        playerModel = new FbxMesh();
        if (!playerModel->Create("Assets/Mannequin/SKM_Manny_Simple.FBX.bin"))
        {
            return false;
        }
        playerModel->LoadAnimation("Idle", "Assets/Mannequin/Animation/MM_Idle.FBX.anm");
        playerModel->LoadAnimation("Walk", "Assets/Mannequin/Animation/MM_Run_Fwd.FBX.anm");
        playerModel->LoadAnimation("Push", "Assets/Mannequin/Animation/MM_Push.FBX.anm");
        playerModel->SetScale({ 0.02f, 0.02f, 0.02f });

        // 初期位置
        playerWorldPos = GridToWorld(game->GetPlayerPos());

        // 床・壁・ゴール・ブロックを生成
        CreateFloor();
        CreateWalls();
        CreateGoals();
        CreateBlocks();

        return true;
    }

    void SokobanRenderer::Release()
    {
        if (playerModel)
        {
            playerModel->Release();
            delete playerModel;
            playerModel = nullptr;
        }

        for (auto* m : blockModels)
        {
            if (m) { m->Release(); delete m; }
        }
        blockModels.clear();

        for (auto* m : floorModels)
        {
            if (m) { m->Release(); delete m; }
        }
        floorModels.clear();

        for (auto* m : wallModels)
        {
            if (m) { m->Release(); delete m; }
        }
        wallModels.clear();

        for (auto* m : goalModels)
        {
            if (m) { m->Release(); delete m; }
        }
        goalModels.clear();
    }

    void SokobanRenderer::Render()
    {
        // 床
        for (auto* m : floorModels)
        {
            if (m) m->Render();
        }

        // 壁
        for (auto* m : wallModels)
        {
            if (m) m->Render();
        }

        // ゴール
        for (auto* m : goalModels)
        {
            if (m) m->Render();
        }

        // ブロック
        for (auto* m : blockModels)
        {
            if (m) m->Render();
        }

        // プレイヤー
        if (playerModel)
        {
            playerModel->SetPosition(playerWorldPos);

            // Y軸回転 + モデルの基本回転（X軸-90度でモデルを立てる）
            constexpr float DEG_TO_RAD = 3.1415926535f / 180.0f;
            Math::Quaternion baseRot = Math::Quaternion::AngleAxis(-90.0f * DEG_TO_RAD, { 1, 0, 0 });
            Math::Quaternion yawRot = Math::Quaternion::AngleAxis(playerRotationY, { 0, 1, 0 });
            playerModel->SetRotation(yawRot * baseRot);

            playerModel->Render();
        }
    }

    void SokobanRenderer::SetPlayerWorldPos(const Math::Vector3& pos)
    {
        playerWorldPos = pos;
    }

    void SokobanRenderer::SetPlayerRotationY(float radians)
    {
        playerRotationY = radians;
    }

    void SokobanRenderer::SetBlockWorldPos(int index, const Math::Vector3& pos)
    {
        if (index >= 0 && index < static_cast<int>(blockModels.size()))
        {
            if (blockModels[index])
            {
                blockModels[index]->SetPosition(pos);
            }
        }
    }

    void SokobanRenderer::PlayPlayerAnimation(const char* name, float dt, bool loop)
    {
        if (playerModel)
        {
            playerModel->Animate(name, dt, loop);
        }
    }

    bool SokobanRenderer::IsPlayerAnimationFinished() const
    {
        if (playerModel)
        {
            return playerModel->GetCurrentAnimationFrameRate() != 0.0f;
        }
        return true;
    }

    Math::Vector3 SokobanRenderer::GridToWorld(const Int2& gridPos)
    {
        return GridToWorld(gridPos.x, gridPos.y);
    }

    Math::Vector3 SokobanRenderer::GridToWorld(int x, int y)
    {
        return {
            x * CELL_SIZE + CELL_SIZE * 0.5f,
            0.0f,
            y * CELL_SIZE + CELL_SIZE * 0.5f
        };
    }

    void SokobanRenderer::CreateFloor()
    {
        const int w = game->GetWidth();
        const int h = game->GetHeight();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                CellType cell = game->GetCell(x, y);
                if (cell == CellType::Wall)
                    continue;

                FbxMesh* floor = new FbxMesh();
                if (floor->Create("Assets/Cube/Cube.fbx.bin"))
                {
                    Math::Vector3 pos = GridToWorld(x, y);
                    pos.y = -0.5f;  // 床は少し下に
                    floor->SetPosition(pos);
                    floor->SetScale({ CELL_SIZE, 1.0f, CELL_SIZE });

                    // ゴールは色を変える
                    if (cell == CellType::Goal)
                    {
                        floor->SetColor(Color::Yellow);
                    }
                    else
                    {
                        floor->SetColor(Color::White);
                    }
                    floorModels.push_back(floor);
                }
                else
                {
                    delete floor;
                }
            }
        }
    }

    void SokobanRenderer::CreateWalls()
    {
        const int w = game->GetWidth();
        const int h = game->GetHeight();

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                if (game->GetCell(x, y) != CellType::Wall)
                    continue;

                FbxMesh* wall = new FbxMesh();
                if (wall->Create("Assets/Cube/Cube.fbx.bin"))
                {
                    Math::Vector3 pos = GridToWorld(x, y);
                    pos.y = CELL_SIZE * 0.5f;  // 壁は床の上
                    wall->SetPosition(pos);
                    wall->SetScale({ CELL_SIZE, CELL_SIZE, CELL_SIZE });
                    wall->SetColor(Color::Gray);
                    wallModels.push_back(wall);
                }
                else
                {
                    delete wall;
                }
            }
        }
    }

    void SokobanRenderer::CreateGoals()
    {
        // ゴールマーカー（床の上に小さなオブジェクト）
        for (const auto& goalPos : game->GetGoalPositions())
        {
            FbxMesh* goal = new FbxMesh();
            if (goal->Create("Assets/Cube/Cube.fbx.bin"))
            {
                Math::Vector3 pos = GridToWorld(goalPos);
                pos.y = 0.05f;  // 床のすぐ上
                goal->SetPosition(pos);
                goal->SetScale({ CELL_SIZE * 0.8f, 0.1f, CELL_SIZE * 0.8f });
                goal->SetColor(Color::Red);
                goalModels.push_back(goal);
            }
            else
            {
                delete goal;
            }
        }
    }

    void SokobanRenderer::CreateBlocks()
    {
        for (const auto& blockPos : game->GetBlockPositions())
        {
            FbxMesh* block = new FbxMesh();
            if (block->Create("Assets/Cube/Cube.fbx.bin"))
            {
                Math::Vector3 pos = GridToWorld(blockPos);
                pos.y = CELL_SIZE * 0.5f;  // 床の上
                block->SetPosition(pos);
                block->SetScale({ CELL_SIZE * 0.9f, CELL_SIZE * 0.9f, CELL_SIZE * 0.9f });
                block->SetColor(Color::Cyan);
                blockModels.push_back(block);
            }
            else
            {
                delete block;
            }
        }
    }
}
