#include "Keyboard.h"

namespace Engine::Input
{
	void Keyboard::Init()
	{
		// 入力場号の初期化
		memset(mCurrent, 0, sizeof(mCurrent));
		memset(mPrev, 0, sizeof(mPrev));
	}

	void Keyboard::Update()
	{
		BYTE keys[KeyMax];
		// キーボードの状態を取得
		if (GetKeyboardState(keys) == false)
		{
			return;
		}

		// 入力情報の更新
		for (int i = 0; i < KeyMax; ++i)
		{
			mPrev[i] = mCurrent[i];
			mCurrent[i] = (keys[i] & 0x80) != 0;
		}
	}

	bool Keyboard::IsPress(int key) const
	{
		if (key < 0 || key >= KeyMax)
		{
			return false;
		}

		return mCurrent[key] == true;
	}

	bool Keyboard::IsPush(int key) const
	{
		if (key < 0 || key >= KeyMax) 
		{
			return false;
		}

		return mPrev[key] == false && mCurrent[key] == true;
	}

	bool Keyboard::IsRelease(int key) const
	{
		if (key < 0 || key >= KeyMax)
		{
			return false;
		}

		return mPrev[key] == true && mCurrent[key] == false;
	}
}
