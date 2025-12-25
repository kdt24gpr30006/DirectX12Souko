#include "PadManager.h"
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt;
using namespace Windows::Gaming::Input;
using namespace Windows::Foundation;

namespace Engine::Input
{
	PadManager::~PadManager()
	{
		if (mAddedToken.value)
		{
			Gamepad::GamepadAdded(mAddedToken);
			Gamepad::GamepadRemoved(mRemovedToken);
		}
	}

	void PadManager::Initialize()
	{
		mPads.clear();

		// 接続されているパッドを取得
		for (const Gamepad& gamepad : Gamepad::Gamepads())
		{
			mPads.emplace_back(gamepad);
		}

		// 接続イベント登録
		mAddedToken = Gamepad::GamepadAdded(
			{ this, &PadManager::OnGamepadAdded });

		// 切断イベント登録
		mRemovedToken = Gamepad::GamepadRemoved(
			{ this, &PadManager::OnGamepadRemoved });
	}

	void PadManager::Update()
	{
		// 全パッド更新
		for (auto& pad : mPads)
		{
			pad.Update();
		}
	}

	bool PadManager::HasPad() const
	{
		return !mPads.empty();
	}

	size_t PadManager::Count() const
	{
		return mPads.size();
	}

	Pad& PadManager::GetPad(size_t index)
	{
		return mPads.at(index);
	}

	const Pad& PadManager::GetPad(size_t index) const
	{
		return mPads.at(index);
	}

	void PadManager::OnGamepadAdded(IInspectable const&, Gamepad const& gamepad)
	{
		// 新しく接続されたパッドを追加
		mPads.emplace_back(gamepad);
	}

	void PadManager::OnGamepadRemoved(IInspectable const&, Gamepad const& gamepad)
	{
		// 切断されたパッドを削除
		std::erase_if(mPads, [&](Pad const& pad)
		{
			return pad.GetGamepad() == gamepad;
		});
	}
}
