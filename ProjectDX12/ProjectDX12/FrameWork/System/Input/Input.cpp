#include "Input.h"
#include <Windows.h>

namespace System
{
	void Input::OnCreate()
	{
		mKeyboard.Initialize();
	}

	void Input::Update()
	{
		mKeyboard.Update();
		mPadManager.Update();
	}

	const Engine::Input::Keyboard& Input::Keyboard() const
	{
		return mKeyboard;
	}

	const Engine::Input::PadManager& Input::Pad() const
	{
		return mPadManager;
	}
}