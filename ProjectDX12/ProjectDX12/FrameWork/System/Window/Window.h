#include <Windows.h>

/// <summary>
/// ウィンドウ用クラス
/// </summary>
class Window
{
public:
	/// <summary>
	/// Windowの横幅
	/// </summary>
	static constexpr float Width = 1280.0f;
	/// <summary>
	/// Windowの縦幅
	/// </summary>
	static constexpr float Height = 720.0f;
public:
	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <returns>true:成功</returns>
	static bool Create();
	/// <summary>
	/// インスタンスの解放
	/// </summary>
	static void Release();
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Window* GetInstance();

	/// <summary>
	/// ウィンドウの初期化
	/// </summary>
	/// <param name="WindowName">ウィンドウ名</param>
	/// <param name="x">x座標</param>
	/// <param name="y">y座標</param>
	/// <param name="isFullScreen">フルスクリーンか?</param>
	/// <returns>true:成功</returns>
	bool Initialize(const char* WindowName, int x, int y);
	/// <summary>
	/// 更新メッセージの取得
	/// </summary>
	/// <returns></returns>
	bool IsUpdateMessage();
	/// <summary>
	/// 終了メッセージ
	/// </summary>
	/// <returns></returns>
	bool IsQuitMessage()const;
	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	const HWND GetWindowHandle()const;
private:
	/// <summary>
	/// ウィンドウハンドル
	/// </summary>
	HWND WindowHandle;
	/// <summary>
	/// 終了通知
	/// </summary>
	bool isQuitMessage;
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	WNDCLASSEX WindowClass;

	/// <summary>
	/// インスタンス
	/// </summary>
	static Window* Instance;

	Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	~Window();
};