#pragma once
#include <winrt/Windows.Gaming.Input.h>

namespace Engine::Input
{
    class Pad
    {
    public:
        // コンストラクタ
        explicit Pad(winrt::Windows::Gaming::Input::Gamepad gamepad);

        // 毎フレーム呼ぶ
        void Update();

        /// <summary>
        /// ボタンが押されているか
        /// </summary>
        /// <param name="button">判定するボタン</param>
        /// <returns>true : 押されている false : 押されていない</returns>
        bool IsPress(winrt::Windows::Gaming::Input::GamepadButtons button) const;

        /// <summary>
        /// ボタンを押した瞬間か
        /// </summary>
        /// <param name="button">判定するボタン</param>
        /// <returns>true : 押した瞬間 false : 押した瞬間じゃない</returns>
        bool IsPush(winrt::Windows::Gaming::Input::GamepadButtons button) const;

        /// <summary>
        /// 離した瞬間か
        /// </summary>
        /// <param name="button">判定するボタン</param>
        /// <returns>true : 離した瞬間 false : 離した瞬間じゃない</returns>
        bool IsRelease(winrt::Windows::Gaming::Input::GamepadButtons button) const;

        // スティック
        float LeftX()  const;
        float LeftY()  const;
        float RightX() const;
        float RightY() const;

        // トリガー
        float LeftTrigger()  const;
        float RightTrigger() const;

        // 状態
        bool IsConnected() const;
        winrt::Windows::Gaming::Input::Gamepad GetGamepad() const;

        /// <summary>
        /// デッドゾーンの設定
        /// </summary>
        /// <param name="argDeadZone"></param>
        void SetDeadZone(float argDeadZone);

    private:

        /// <summary>
        /// デッドゾーン
        /// </summary>
        /// <param name="v">スティックの移動量</param>
        /// <returns></returns>
        float DeadZone(float v) const;

    private:
        // パッド本体情報
        winrt::Windows::Gaming::Input::Gamepad mGamepad = nullptr;

        // パッドの入力状態
        winrt::Windows::Gaming::Input::GamepadReading mReading{};

        // 現在のボタン状態
        winrt::Windows::Gaming::Input::GamepadButtons mCurrent =
            winrt::Windows::Gaming::Input::GamepadButtons::None;

        // 前フレームのボタン状態
        winrt::Windows::Gaming::Input::GamepadButtons mPrevious =
            winrt::Windows::Gaming::Input::GamepadButtons::None;

        // 接続状態
        bool mConnected = true;

        float mDeadZoen = 0.2f;
    };
}
