#pragma once

class Player;

class StateMachine;

class StateBase
{
public:
    virtual ~StateBase() = default;

    /// <summary>
    /// ステートマシンを設定
    /// </summary>
    /// <param name="sm"></param>
    void SetStateMachine(StateMachine* sm) { stateMachine = sm; }

    /// <summary>
    /// ステートに入ったとき
    /// </summary>
    /// <param name=""></param>
    virtual void Enter(Player*) {}

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name=""></param>
    /// <param name=""></param>
    virtual void Update(Player*, float) = 0;

    /// <summary>
    /// ステートを抜けるとき
    /// </summary>
    /// <param name=""></param>
    virtual void Exit(Player*) {}

protected:

    // ステートマシンの参照
    StateMachine* stateMachine = nullptr;
};
