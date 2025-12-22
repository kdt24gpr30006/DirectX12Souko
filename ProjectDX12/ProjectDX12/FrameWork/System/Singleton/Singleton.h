#pragma once

#ifdef GENERATE_SINGLETON_BODY
#undef GENERATE_SINGLETON_BODY
#endif

/*
* シングルトン用基本コードの定義
* コード例:
* class Temp : Singleton<Temp>
* {
*	GENERATE_SINGLETON_BODY(Temp)
*	[ ~以下クラス処理の実装... ]
* }
*/
#define GENERATE_SINGLETON_BODY(Class)\
private:\
friend System::Singleton<Class>;\
Class()noexcept = default;\
~Class()noexcept  = default;\
Class(const Class&) = delete;\
Class(Class&&) = delete;\
Class& operator=(const Class&) = delete;\
Class& operator=(Class&&) = delete;


namespace System
{
	/// <summary>
	/// シングルトンテンプレート
	/// </summary>
	/// <typeparam name="T">シングルトン用クラス</typeparam>
	template<class T>
	class Singleton
	{
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>true:成功</returns>
		static bool Create()
		{
			if (Instance == nullptr)
			{
				Instance = new T();
				Instance->OnCreate();
				return true;
			}
			return false;
		}

		/// <summary>
		/// インスタンス解放
		/// </summary>
		static void Release()
		{
			if (Instance != nullptr)
			{
				Instance->OnDestroy();
				delete Instance;
				Instance = nullptr;
			}
		}

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns></returns>
		static T* GetInstance()
		{
			return Instance;
		}

	protected:

		/// <summary>
		/// インスタンス生成時の処理
		/// </summary>
		virtual void OnCreate()
		{
		}

		/// <summary>
		/// インスタンスの解放時の処理
		/// </summary>
		virtual void OnDestroy()
		{
		}

		Singleton() noexcept
		{
		}
		virtual ~Singleton() noexcept
		{
		}
	private:

		static T* Instance;

		Singleton(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton& operator=(Singleton&&) = delete;
	};

	template<class T>
	T* Singleton<T>::Instance = nullptr;
}