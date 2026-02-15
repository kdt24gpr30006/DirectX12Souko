#pragma once

#include "../Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"
#include <Math/Quaternion/Quaternion.h>
#include "System/Collider/AABB/AABBCollider.h"

class Stage;
class Block;
class FbxMesh;
class CharaStateMachine;
class CameraWork;
class BlobShadow;
class Texture;

class Player : public Entity
{
public:
    // 移動速度
    static constexpr float MoveSpeed = 15.0f;

    Player();
    ~Player();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="stage"></param>
    /// <returns></returns>
    bool Init(Stage* stage, const Int2 startGrid);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="dt"></param>
    void Update(float dt) override;

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void Release() override;

    /// <summary>
    /// stageのポインターを返す
    /// </summary>
    Stage* GetStage() const { return stage; }

    /// <summary>
    /// CameraWorkを設定
    /// </summary>
    void SetCameraWork(CameraWork* cam) { cameraWork = cam; }

    /// <summary>
    /// CameraWorkを取得
    /// </summary>
    CameraWork* GetCameraWork() const { return cameraWork; }

    /// <summary>
    /// 指定した名前のアニメーションを再生
    /// </summary>
    /// <param name="name"></param>
    /// <param name="dt"></param>
    /// <param name="loop"></param>
    void PlayAnimation(const char* name, float dt, bool loop);

    /// <summary>
    /// 向きの更新用
    /// </summary>
    /// <param name="dir"></param>
    void SetFacingDirection(const Math::Vector3& dir);

    void SetPushing(bool v) { isPushing = v; }

    /// <summary>
    /// 前方向を返す
    /// </summary>
    /// <returns></returns>
    const Math::Vector3& GetForward() const;

    /// <summary>
    /// 現在のgridを返す
    /// </summary>
    /// <returns></returns>
    Int2 GetGridPos() const;

    /// <summary>
    /// コライダーを取得
    /// </summary>
    /// <returns></returns>
    const AABBCollider* GetCollider() const { return &collider; }

    /// <summary>
    /// コライダー位置を更新
    /// </summary>
    void UpdateCollider();

    /// <summary>
    /// プレイヤーを初期位置にリセット（メッシュは保持）
    /// </summary>
    void ResetToStart(const Int2& startGrid);

private:

    /// <summary>
    /// 入力に基づいて向きを更新
    /// </summary>
    void UpdateFacingFromInput();

    /// <summary>
    /// ブロックを押したときのイベント
    /// </summary>
    void TryPushBlock();

    Stage* stage = nullptr;
    CharaStateMachine* stateMachine = nullptr;
    CameraWork* cameraWork = nullptr;

    // 足元の影
    BlobShadow* shadow = nullptr;
    Texture* shadowTexture = nullptr;

    // 当たり判定用コライダー
    AABBCollider collider;

    // Push中は入力による向き変更をスキップ
    bool isPushing = false;

    // 押す方向用の向き
    Int2 facingDir{ 0, 1 };

    //  描画用 forward キャッシュ
    mutable Math::Vector3 ForwardCache;

    // 滑らかな回転用のターゲット回転
    Math::Quaternion targetRotation;

    // 回転補間速度（大きいほど速く振り向く）
    static constexpr float TurnSpeed = 6.0f;

    // UEから持ってきたモデルは回転しているので補正用
    constexpr static float DEG_TO_RAD = 3.1415926535f / 180.0f;
    Math::Quaternion x90 =
        Math::Quaternion::AngleAxis(270.0f * DEG_TO_RAD, Math::Vector3{ 1.0f, 0.0f, 0.0f });
};