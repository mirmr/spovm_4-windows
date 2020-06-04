#pragma once
#include <functional>
#include <Windows.h>
#include <iostream>

namespace mns
{
	//template<class F>
	class thread
	{
	private:

		DWORD id;
		HANDLE handle;
		bool joined;
		bool closed;

		template<class F>
		static DWORD WINAPI func(LPVOID lparams);
	public:
		template<class F>
		explicit thread(F function);

		thread(const thread& other) = delete;
		thread(thread&& other) noexcept;
		thread operator=(const thread& other) = delete;
		const thread& operator=(thread&& other) = delete;

		void join();
		bool try_join();
		~thread();
	};	

	template<class F>
	inline thread::thread(F function)
		:joined{ false }, closed{ false }
	{
		static F funct = function;
		handle = CreateThread(NULL, 0, func<F>, &funct, NULL, &id);
		if (handle == NULL)
		{
			throw std::exception{};
		}
	}

	inline thread::thread(thread&& other) noexcept
		:id{ other.id }, handle{ other.handle }, joined{ other.joined }, closed{ other.closed }
	{
		other.closed = true;
		other.joined = true;
	}

	inline void thread::join()
	{
		if (joined)
		{
			return;
		}
		WaitForSingleObject(handle, INFINITE);
		joined = true;
	}

	inline bool thread::try_join()
	{
		if (joined)
			return true;
		if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0)
		{
			joined = true;
			return true;
		}
		return false;
	}

	inline thread::~thread()
	{
		if (!joined)
		{
			throw std::exception{};
		}
		if (!closed)
		{
			CloseHandle(handle);
			closed = true;
		}
	}

	template<class F>
	inline DWORD __stdcall thread::func(LPVOID lparams)
	{
		F function = *(F*)(lparams);
		function();
		return 0;
	}
}
