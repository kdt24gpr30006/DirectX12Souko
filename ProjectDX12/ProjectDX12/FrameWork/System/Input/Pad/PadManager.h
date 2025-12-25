#pragma once
#include <vector>
#include <winrt/Windows.Gaming.Input.h>
#include "Pad.h"

namespace Engine::Input
{
	class PadManager
	{
	public:
		PadManager() = default;
		~PadManager();


		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update();

		/// <summary>
		/// ゲームパッドが1台以上接続されているか
		/// </summary>
		/// <returns>true : 1台以上存在する / false : 存在しない</returns>
		bool HasPad() const;

		/// <summary>
		/// 接続されているゲームパッドの数を取得
		/// </summary>
		size_t Count() const;

		/// <summary>
		/// 指定されたインデックスのパッドを取得（const版）
		/// </summary>
		Pad& GetPad(size_t index = 0);

		// const版
		const Pad& GetPad(size_t index = 0) const;

	private:
		/// <summary>
		/// パッド接続時に呼ばれる
		/// </summary>
		void OnGamepadAdded(
			winrt::Windows::Foundation::IInspectable const&,
			winrt::Windows::Gaming::Input::Gamepad const&);

		/// <summary>
		/// パッド切断時に呼ばれる
		/// </summary>
		void OnGamepadRemoved(
			winrt::Windows::Foundation::IInspectable const&,
			winrt::Windows::Gaming::Input::Gamepad const&);

	private:

		/// <summary>
		/// 接続中のゲームパッド一覧
		/// </summary>
		std::vector<Pad> mPads;

		winrt::event_token mAddedToken{};
		winrt::event_token mRemovedToken{};
	};
}
