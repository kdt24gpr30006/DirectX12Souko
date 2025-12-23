#include "Pad.h"

using namespace winrt;
using namespace Windows::Gaming::Input;

namespace Engine::Input
{
    Pad::Pad(Gamepad gamepad)
        : mGamepad(gamepad)
    {
    }

    void Pad::Update()
    {
        if (!mGamepad)
        {
            mConnected = false;
            return;
        }

        // 前フレーム保存
        mPrevious = mCurrent;

        // 現在の状態取得
        mReading = mGamepad.GetCurrentReading();
        mCurrent = mReading.Buttons;
    }
    

    bool Pad::IsPress(GamepadButtons button) const
    {
        return (mCurrent & button) != GamepadButtons::None;
    }

    bool Pad::IsPush(GamepadButtons button) const
    {
        return ((mPrevious & button) == GamepadButtons::None) &&
            ((mCurrent & button) != GamepadButtons::None);
    }

    bool Pad::IsRelease(GamepadButtons button) const
    {
        return ((mPrevious & button) != GamepadButtons::None) &&
            ((mCurrent & button) == GamepadButtons::None);
    }

    float Pad::DeadZone(float v) const
    {
        // デッドゾーン内なら0を返す
        return (std::abs(v) < mDeadZoen) ? 0.0f : v;
    }

    float Pad::LeftX() const
    {
        return DeadZone(static_cast<float>(mReading.LeftThumbstickX));
    }

    float Pad::LeftY() const
    {
        return DeadZone(static_cast<float>(mReading.LeftThumbstickY));
    }

    float Pad::RightX() const
    {
        return DeadZone(static_cast<float>(mReading.RightThumbstickX));
    }

    float Pad::RightY() const
    {
        return DeadZone(static_cast<float>(mReading.RightThumbstickY));
    }

    float Pad::LeftTrigger() const
    {
        return static_cast<float>(mReading.LeftTrigger);
    }

    float Pad::RightTrigger() const
    {
        return static_cast<float>(mReading.RightTrigger);
    }

    bool Pad::IsConnected() const
    {
        return mGamepad != nullptr;
    }

    Gamepad Pad::GetGamepad() const
    {
        return mGamepad;
    }
    void Pad::SetDeadZone(float argDeadZone)
    {
        mDeadZoen = argDeadZone;
    }
}
