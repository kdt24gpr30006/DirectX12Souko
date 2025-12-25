#pragma once
#include "System/Singleton/Singleton.h"
#include "Keyboard/Keyboard.h"
#include "Pad/PadManager.h"

namespace System
{
	/// <summary>
	/// 入力系管理クラス
	/// </summary>
	class Input : public Singleton<Input>
	{
		GENERATE_SINGLETON_BODY(Input)

	protected:
		/// <summary>
		/// 初期化
		/// </summary>
		void OnCreate() override;

	public:

		/// <summary>
		/// キー入力情報の更新
		/// </summary>
		void Update();

		/// <summary>
		/// キーボードのゲッター
		/// </summary>
		/// <returns></returns>
		const Engine::Input::Keyboard& Keyboard() const;

		/// <summary>
		/// パッドのゲッター
		/// </summary>
		/// <returns></returns>
		const Engine::Input::PadManager& Pad() const;

	private:

		Engine::Input::Keyboard mKeyboard;
		Engine::Input::PadManager mPadManager;
	};
}