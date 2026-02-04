#pragma once
#include <Windows.h>

// 高精度タイマークラス
class GameTimer
{
public:
	GameTimer()
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
	}

	// デルタタイムを計算して返す（秒単位）
	float Tick()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart)
			/ static_cast<float>(frequency.QuadPart);

		lastTime = currentTime;

		// デルタタイムが大きすぎる場合はキャップ（フレーム落ち対策）
		if (deltaTime > MAX_DELTA_TIME)
		{
			deltaTime = MAX_DELTA_TIME;
		}

		return deltaTime;
	}

private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER lastTime;

	// 最大デルタタイム（約10FPS相当）
	static constexpr float MAX_DELTA_TIME = 0.1f;
};