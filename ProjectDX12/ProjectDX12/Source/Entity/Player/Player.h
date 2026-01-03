#pragma once
#include "../../Entity/Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"

class FbxMesh;
class StateMachine;
class Stage;
class Block;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Entity
{
public:
    // 移動速度
    static constexpr float MoveSpeed = 4.0f;
    // ブロックを押せる範囲
    static constexpr float PushRange = 0.7f;

public:
    Player();
    ~Player();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="stage"></param>
    /// <returns></returns>
    bool Initialize(Stage* stage);

    /// <summary>
    /// 解放
    /// </summary>
    void Release();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="dt"></param>
    void Update(float dt) override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;

    /// <summary>
    /// アニメーション
    /// </summary>
    /// <param name="name"></param>
    /// <param name="dt"></param>
    /// <param name="loop"></param>
    void PlayAnimation(const char* name, float dt, bool loop);

    /// <summary>
    /// Push可能か判定
    /// </summary>
    /// <param name="outBlock"></param>
    /// <param name="outDir"></param>
    /// <returns></returns>
    bool CanPush(Block*& outBlock, Int2& outDir) const;

    /// <summary>
    /// プレイヤーに関するステージを取得
    /// </summary>
    /// <returns></returns>
    Stage* GetStage() const;

protected:
    /// <summary>
    /// オブジェクトの半分のサイズを取得する
    /// </summary>
    /// <returns></returns>
    Math::Vector3 GetHalfSize() const override;

private:
    // 見た目
    FbxMesh* model = nullptr;

    // ステート
    StateMachine* stateMachine = nullptr;

    // ステージ参照
    Stage* stage = nullptr;
};
