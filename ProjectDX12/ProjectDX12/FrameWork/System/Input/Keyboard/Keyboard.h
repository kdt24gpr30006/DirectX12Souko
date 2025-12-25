#pragma once
#include <Windows.h>

namespace Engine::Input
{
	class Keyboard
	{
		/// <summary>
		/// キーボードの最大入力検知数
		/// </summary>
		static constexpr int KeyMax = 256;

	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// キー入力情報の更新
		/// </summary>
		void Update();

		/// <summary>
		/// キーが押されているか
		/// </summary>
		/// <param name="index">判定するキー</param>
		/// <returns>true : 押されている false : 押されていない</returns>
		bool IsPress(int key) const;

		/// <summary>
		/// キーを押した瞬間か
		/// </summary>
		/// <param name="index">判定するキー</param>
		/// <returns>true : 押した瞬間 false : 押した瞬間じゃない</returns>
		bool IsPush(int key) const;

		/// <summary>
		/// キーを離した瞬間か
		/// </summary>
		/// <param name="index">判定するキー</param>
		/// <returns>true : 離した瞬間 false : 離した瞬間じゃない</returns>
		bool IsRelease(int key) const;

	private:
		/// <summary>
		/// 現在の入力情報
		/// </summary>
		bool mCurrent[KeyMax]{};

		/// <summary>
		/// 前の入力情報
		/// </summary>
		bool mPrev[KeyMax]{};
	};
}
