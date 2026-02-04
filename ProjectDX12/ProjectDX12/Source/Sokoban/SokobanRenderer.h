#pragma once
#include <vector>
#include <memory>
#include "SokobanTypes.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"

class FbxMesh;

namespace Sokoban
{
    class SokobanGame;

    // 3D描画を担当
    class SokobanRenderer
    {
    public:
        // グリッド1マスのワールドサイズ
        static constexpr float CELL_SIZE = 2.0f;

        SokobanRenderer() = default;
        ~SokobanRenderer();

        // 初期化（ゲーム状態を参照）
        bool Initialize(const SokobanGame* game);

        // 解放
        void Release();

        // 描画
        void Render();

        // プレイヤー位置を更新（アニメーション用）
        void SetPlayerWorldPos(const Math::Vector3& pos);

        // プレイヤー回転を更新
        void SetPlayerRotationY(float radians);

        // ブロック位置を更新（アニメーション用）
        void SetBlockWorldPos(int index, const Math::Vector3& pos);

        // プレイヤーアニメーション再生
        void PlayPlayerAnimation(const char* name, float dt, bool loop);

        // プレイヤーアニメーション終了判定
        bool IsPlayerAnimationFinished() const;

        // グリッド座標→ワールド座標変換
        static Math::Vector3 GridToWorld(const Int2& gridPos);
        static Math::Vector3 GridToWorld(int x, int y);

    private:
        void CreateFloor();
        void CreateWalls();
        void CreateGoals();
        void CreateBlocks();

    private:
        const SokobanGame* game = nullptr;

        // モデル
        FbxMesh* playerModel = nullptr;
        std::vector<FbxMesh*> blockModels;
        std::vector<FbxMesh*> floorModels;
        std::vector<FbxMesh*> wallModels;
        std::vector<FbxMesh*> goalModels;

        // プレイヤーのワールド位置（アニメーション用）
        Math::Vector3 playerWorldPos{ 0, 0, 0 };
        float playerRotationY = 0.0f;
    };
}
