#pragma once
#include <stdint.h>

namespace System
{
	class Memory
	{
	public:
		/// <summary>
		/// KiBを計算
		/// </summary>
		/// <param name="Size"></param>
		/// <returns></returns>
		static constexpr size_t KiB(const size_t Size)
		{
			return Size * 1024;
		}

		/// <summary>
		/// MiBを計算
		/// </summary>
		/// <param name="Size"></param>
		/// <returns></returns>
		static constexpr size_t MiB(const size_t Size)
		{
			return KiB(Size) * 1024;
		}

		/// <summary>
		/// GiBを計算
		/// </summary>
		/// <param name="Size"></param>
		/// <returns></returns>
		static constexpr size_t GiB(const size_t Size)
		{
			return MiB(Size) * 1024;
		}
	};
}