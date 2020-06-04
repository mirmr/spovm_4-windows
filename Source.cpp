#include "thread.h"
#include <iostream>
#include <conio.h>
#include <atomic>
#include <list>

void slave();

#define SHIFTED 0x8000
CRITICAL_SECTION io_sync;
bool to_exit;
std::atomic<int> left_to_leave;

int main()
{
	InitializeCriticalSection(&io_sync);
	std::list<mns::thread> threads;

	char val = 0;
	bool p_pressed = false;
	bool m_pressed = false;
	bool p_prev_state = false;
	bool m_prev_state = false;

	while (val != 'q')
	{
		if (GetKeyState(VK_OEM_PLUS) & SHIFTED)
		{
			if (!p_prev_state)
			{
				p_pressed = true;
			}
			p_prev_state = true;
		}
		else
		{
			p_prev_state = false;
		}


		if (GetKeyState(VK_OEM_MINUS) & SHIFTED)
		{
			if (!m_prev_state)
			{
				m_pressed = true;
			}
			m_prev_state = true;
		}
		else
		{
			m_prev_state = false;
		}

		if (p_pressed)
		{
			p_pressed = false;
			threads.push_back(std::move(mns::thread{ slave }));
		}

		if (m_pressed)
		{
			m_pressed = false;
			if (!threads.empty())
			{
				left_to_leave = 1;
				to_exit = true;
				bool joined = false;
				while (!joined)
				{
					for (auto i = threads.begin(); i != threads.end(); i++)
					{
						if ((*i).try_join())
						{
							threads.erase(i);
							joined = true;
							break;
						}
					}
				}
				//std::cout << "deleted" << std::endl;
				to_exit = false;
			}
		}

		if (GetKeyState('Q') & SHIFTED)
		{
			break;
		}
	}

	to_exit = true;
	left_to_leave = threads.size();

	for (auto& i : threads)
	{
		i.join();
	}

	DeleteCriticalSection(&io_sync);

	return 0;
}

void slave()
{
	while (true)
	{
		if (to_exit)
		{
			int m_count = left_to_leave--;
			if (m_count > 0)
			{
				break;
			}
		}
		EnterCriticalSection(&io_sync);
		std::cout << 't' << 'h' << 'r' << 'e' << 'a' << 'd' << ' ' << GetCurrentThreadId() << std::endl;
		LeaveCriticalSection(&io_sync);
		Sleep(1);
	}
}
