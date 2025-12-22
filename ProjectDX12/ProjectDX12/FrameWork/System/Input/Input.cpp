#include "Input.h"
#include <Windows.h>

namespace System
{
	/// <summary>
	/// 初期化
	/// </summary>
	void Input::OnCreate()
	{
		for (int i = 0; i < KeyMax; i++)
		{
			CurrentKeyState[i] = false;
			PrevKeyState[i] = false;
		}
	}

	/// <summary>
	/// キー入力情報の更新
	/// </summary>
	void Input::Update()
	{
		BYTE KeyState[KeyMax];
		const BOOL ret = GetKeyboardState(KeyState);
		if (ret == FALSE)
		{
			return;
		}

		for (int i = 0; i < KeyMax; i++)
		{
			PrevKeyState[i] = CurrentKeyState[i];
			//最上位ビットのチェック
			CurrentKeyState[i] = KeyState[i] & 0b10000000;
		}
	}

	/// <summary>
	/// キーが押されているか?
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	bool Input::IsPress(int index)const
	{
		return CurrentKeyState[index];
	}

	/// <summary>
	/// キーを押した瞬間か?
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	bool Input::IsPush(int index)const
	{
		const bool bCurrent = CurrentKeyState[index];
		const bool bPrev = PrevKeyState[index];
		return bPrev == false && bCurrent == true;
	}

	/// <summary>
	/// キーを離した瞬間か?
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	bool Input::IsRelease(int index)const
	{
		const bool bCurrent = CurrentKeyState[index];
		const bool bPrev = PrevKeyState[index];
		return bPrev == true && bCurrent == false;
	}
}