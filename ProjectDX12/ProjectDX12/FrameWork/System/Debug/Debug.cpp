#include "Debug.h"
#include "System/Window/Window.h"
#include <io.h>
#include <Fcntl.h>
#include <iostream>
#include <string.h>
#include <string>

FILE* Debug::ConOutFP = nullptr;
FILE* Debug::ConInFP = nullptr;

static bool operator&(const EConsoleTextColor a, const EConsoleTextColor b)
{
	return static_cast<uint8_t>(a) & static_cast<uint8_t>(b);
}

/// <summary>
/// 初期化
/// </summary>
void Debug::Init()
{
	AllocConsole();
	freopen_s(&ConOutFP, "CONOUT$", "w", stdout);
	freopen_s(&ConInFP, "CONIN$", "r", stdin);
}

/// <summary>
/// 解放時の処理
/// </summary>
void Debug::Release()
{
	if (ConOutFP != nullptr)
	{
		fclose(ConOutFP);
	}
	if (ConInFP != nullptr)
	{
		fclose(ConInFP);
	}
	FreeConsole();
}

/// <summary>
/// コンソールへの出力
/// この関数を直接呼ばずに「DEBUG_LOG」マクロを呼ぶことを推奨
/// </summary>
/// <param name="sorcefile">__FILE__</param>
/// <param name="line">__LINE__</param>
/// <param name="level">ログの出力レベル</param>
/// <param name="text">出力テキスト</param>
/// <param name="">可変引数</param>
void Debug::Log(const char* sorcefile, int line, ELogLevel level, const char* text, ...)
{
	//	ソースファイルの箇所の記録
	char sourcefile[256] = {};
	sprintf_s(sourcefile, "%s(%d)\n", sorcefile, line);

	//	可変長引数の文字列
	va_list list = nullptr;
	va_start(list, text);
	char temp[256] = {};
	vsprintf_s(temp, text, list);
	va_end(list);

	std::string message = {};
	message.reserve(512);
	
	auto SetTextColor = [](const EConsoleTextColor ConsoleTextColor)
		{
			WORD Color = 0;
			if (ConsoleTextColor & EConsoleTextColor::Red)
			{
				Color |= FOREGROUND_RED;
			}
			if (ConsoleTextColor & EConsoleTextColor::Blue)
			{
				Color |= FOREGROUND_BLUE;
			}
			if (ConsoleTextColor & EConsoleTextColor::Green)
			{
				Color |= FOREGROUND_GREEN;
			}

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, Color);
		};

	switch (level)
	{
	case ELogLevel::Log://通常ログ
	{
		OutputDebugString(temp);
		break;
	}
	case ELogLevel::Warning://警告ログ
	{
		message = "Warning : ";
		message += temp;
		OutputDebugString(message.c_str());
		SetTextColor(EConsoleTextColor::Yellow);
		printf_s(message.c_str());
		OutputDebugString(sourcefile);
		break;
	}
	case ELogLevel::Error://エラーログ
	{
		message = "Error : ";
		message += temp;
		OutputDebugString(message.c_str());
		SetTextColor(EConsoleTextColor::Red);
		printf_s(message.c_str());
		OutputDebugString(sourcefile);
		break;
	}
	case ELogLevel::Fatal://実行停止
	{
		OutputDebugString(sourcefile);

		message = temp;
		message += sourcefile;
		Window* window = Window::GetInstance();
		MessageBox(window->GetWindowHandle(),
			message.c_str(), "FatalError", MB_ICONERROR);
		
		//ブレークポイント例外の発生(実行を止める)
		DebugBreak();
		break;
	}
	default:
		break;
	}
}