#pragma once

#include "../Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"

class Stage;
class Block;
class FbxMesh;
class CharaStateMachine;

class Player : public Entity
{
public:
    // 移動速度
    static constexpr float MoveSpeed = 10.0f;

    Player();
    ~Player();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="stage"></param>
    /// <returns></returns>
    bool Init(Stage* stage);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="dt"></param>
    void Update(float dt) override;

    /// <summary>
    /// 終了
    /// </summary>
    void Release() override;

    /// <summary>
    /// stageのポインターを返す
    /// </summary>
    /// <returns></returns>
    Stage* GetStage() const { return stage; }

    ///// <summary>
    ///// 押せるか判定し、押せるならそのブロックと方向を返す
    ///// </summary>
    ///// <param name="outBlock"></param>
    ///// <param name="outDir"></param>
    ///// <returns></returns>
    //bool CanPush(Block*& outBlock, Int2& outDir) const;

    /// <summary>
    /// 指定した名前のアニメーションを再生する
    /// </summary>
    /// <param name="name"></param>
    /// <param name="dt"></param>
    /// <param name="loop"></param>
    void PlayAnimation(const char* name, float dt, bool loop);

    /// <summary>
    /// 前方方向を返す
    /// </summary>
    /// <returns></returns>
    const Math::Vector3& GetForward() const;

    /// <summary>
    /// 今いるgridを返す
    /// </summary>
    /// <returns></returns>
    Int2 GetGridPos() const;

private:

    /// <summary>
    /// 
    /// </summary>
    void UpdateFacingFromInput();

    /// <summary>
    /// ブロックをおそうとするイベント
    /// </summary>
    void TryPushBlock();

    Stage* stage = nullptr;
    CharaStateMachine* stateMachine = nullptr;

    // 倉庫番用の向き
    Int2 facingDir{ 0, 1 };

    // 見た目用 forward キャッシュ
    mutable Math::Vector3 ForwardCache;
};