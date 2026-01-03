#pragma once

class Player;

class StateBase;

/// <summary>
/// ステートマシン
/// </summary>
class StateMachine
{
public:
    ~StateMachine();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="owner"></param>
    /// <param name="start"></param>
    void Initialize(Player* owner, StateBase* start);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="owner"></param>
    /// <param name="dt"></param>
    void Update(Player* owner, float dt);

    /// <summary>
    /// ステートの変更
    /// </summary>
    /// <param name="owner"></param>
    /// <param name="next"></param>
    void ChangeState(Player* owner, StateBase* next);

private:

    // 現在のステート
    StateBase* current = nullptr;
};
