#include "Mouse.h"

namespace Engine
{
    namespace Input
    {
        void Mouse::Init(HWND inHwnd)
        {
            hwnd = inHwnd;

            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            currentX = pt.x;
            currentY = pt.y;
            prevX = currentX;
            prevY = currentY;
            deltaX = 0;
            deltaY = 0;
        }

        void Mouse::Update()
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);

            prevX = currentX;
            prevY = currentY;
            currentX = pt.x;
            currentY = pt.y;

            deltaX = currentX - prevX;
            deltaY = currentY - prevY;

            // マウスボタン状態
            leftDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
            rightDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
            middleDown = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

            // ホイール値を取得してリセット
            wheelDelta = wheelDeltaAccum;
            wheelDeltaAccum = 0;

            // ロック中はウィンドウ中央に戻す
            if (isLocked && hwnd)
            {
                RECT rect;
                GetClientRect(hwnd, &rect);
                int centerX = (rect.right - rect.left) / 2;
                int centerY = (rect.bottom - rect.top) / 2;

                POINT center = { centerX, centerY };
                ClientToScreen(hwnd, &center);
                SetCursorPos(center.x, center.y);

                // 中央に戻したので次フレームの基準を更新
                prevX = centerX;
                prevY = centerY;
                currentX = centerX;
                currentY = centerY;
            }
        }

        void Mouse::SetCursorVisible(bool visible)
        {
            if (cursorVisible != visible)
            {
                cursorVisible = visible;
                ShowCursor(visible ? TRUE : FALSE);
            }
        }

        void Mouse::SetLocked(bool locked)
        {
            isLocked = locked;
            SetCursorVisible(!locked);
        }
    }
}
