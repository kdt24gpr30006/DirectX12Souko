#pragma once
#include <stdio.h>
#include <stdint.h>

/// <summary>
/// デバッグ出力のレベル定義
/// </summary>
enum class ELogLevel : uint8_t
{
	/// <summary>
	/// 通常ログ 
	/// 出力先:出力ログ
	/// </summary>
	Log,

	/// <summary>
	/// 警告ログ 
	/// 出力先:出力ログ, コンソール画面
	/// </summary>
	Warning,

	/// <summary>
	/// エラーログ 
	/// 出力先:出力ログ, コンソール画面
	/// </summary>
	Error,

	/// <summary>
	/// 致命的エラー(実行をその場で止める) 
	/// 出力先:出力ログ, コンソール画面
	/// </summary>
	Fatal
};

enum class EConsoleTextColor : uint8_t
{
	Red = 0x01,
	Blue = 0x02,
	Green = 0x04,
	Yellow = Red | Green,
	Purple = Red | Blue,
	Cyan = Blue | Green,
	White = Red | Blue | Green,
};

/// <summary>
/// デバッグ用クラス
/// </summary>
class Debug
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	static void Init();

	/// <summary>
	/// 解放時の処理
	/// </summary>
	static void Release();

	/// <summary>
	/// コンソールへの出力
	/// この関数を直接呼ばずに「DEBUG_LOG」マクロを呼ぶことを推奨
	/// </summary>
	/// <param name="sorcefile">__FILE__</param>
	/// <param name="line">__LINE__</param>
	/// <param name="level">ログの出力レベル</param>
	/// <param name="text">出力テキスト</param>
	/// <param name="">可変引数</param>
	static void Log(const char* sorcefile, int line, ELogLevel level, const char* text, ...);

private:

	static FILE* ConOutFP;
	static FILE* ConInFP;
};

/*
* __VA_OPT__ は C++20 の機能
* プロジェクトの「プロパティ」>「コマンドライン」から　"/Zc:preprocessor"の追加が必要
*/

//	ログ出力
#define DEBUG_LOG(LogLevel, Text, ...)\
Debug::Log(__FILE__, __LINE__, LogLevel, Text __VA_OPT__(,) __VA_ARGS__);