#pragma once
#include "System/Singleton/Singleton.h"
#include <Windows.h>

namespace System
{
	/// <summary>
	/// 入力系管理クラス
	/// </summary>
	class Input : public Singleton<Input>
	{
		GENERATE_SINGLETON_BODY(Input)

		/// <summary>
		/// キーボードの最大入力検知数
		/// </summary>
		static constexpr int KeyMax = 256;
	protected:
		/// <summary>
		/// 初期化
		/// </summary>
		void OnCreate()override;

	public:

		/// <summary>
		/// キー入力情報の更新
		/// </summary>
		void Update();

		/// <summary>
		/// キーが押されているか?
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsPress(int index)const;

		/// <summary>
		/// キーを押した瞬間か?
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsPush(int index)const;

		/// <summary>
		/// キーを離した瞬間か?
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		bool IsRelease(int index)const;

	private:

		/// <summary>
		/// 現在の入力情報
		/// </summary>
		bool CurrentKeyState[KeyMax] = {};

		/// <summary>
		/// 前の入力情報
		/// </summary>
		bool PrevKeyState[KeyMax] = {};
	};
}