#pragma once
#include <Windows.h>

namespace Engine
{
    namespace Input
    {
        class Mouse
        {
        public:
            Mouse() = default;

            void Init(HWND hwnd);
            void Update();

            // マウス移動量（前フレームからの差分）
            int GetDeltaX() const { return deltaX; }
            int GetDeltaY() const { return deltaY; }

            // マウス位置（スクリーン座標）
            int GetX() const { return currentX; }
            int GetY() const { return currentY; }

            // マウスボタン
            bool IsLeftDown() const { return leftDown; }
            bool IsRightDown() const { return rightDown; }
            bool IsMiddleDown() const { return middleDown; }

            // マウスホイール（+:上、-:下）
            int GetWheelDelta() const { return wheelDelta; }

            // マウスカーソルの表示/非表示
            void SetCursorVisible(bool visible);

            // マウスをウィンドウ中央に固定
            void SetLocked(bool locked);
            bool IsLocked() const { return isLocked; }

            // ホイール値を設定（Windowメッセージから呼ばれる）
            void SetWheelDelta(int delta) { wheelDeltaAccum += delta; }

        private:
            HWND hwnd = nullptr;
            int currentX = 0;
            int currentY = 0;
            int prevX = 0;
            int prevY = 0;
            int deltaX = 0;
            int deltaY = 0;
            int wheelDelta = 0;
            int wheelDeltaAccum = 0;
            bool leftDown = false;
            bool rightDown = false;
            bool middleDown = false;
            bool isLocked = false;
            bool cursorVisible = true;
        };
    }
}
