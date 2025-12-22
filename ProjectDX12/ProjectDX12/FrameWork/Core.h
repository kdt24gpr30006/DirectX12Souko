#pragma once

namespace Core
{
	/// <summary>
	/// フレームワークの初期化
	/// </summary>
	/// <returns></returns>
	bool Initialize();
	/// <summary>
	/// フレームワークの解放
	/// </summary>
	void Release();

	/// <summary>
	/// 新規フレームの開始
	/// </summary>
	void NewFrame();

	/// <summary>
	/// レンダリングの開始
	/// </summary>
	void BegineRendering();

	/// <summary>
	/// フレームの終了
	/// </summary>
	void EndFrame();
}