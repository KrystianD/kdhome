#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <sys/time.h>
#include <stdint.h>

class Timer
{
	public:
		Timer () : m_intervalSeconds (0),  m_intervalMicroseconds (0), m_lastCheck (-1) { }
		Timer (uint32_t intervalSeconds) : m_intervalSeconds (intervalSeconds), m_intervalMicroseconds (0), m_lastCheck (-1) { }
		Timer (uint32_t intervalSeconds, uint32_t intervalMicroseconds) : m_intervalSeconds (intervalSeconds), m_intervalMicroseconds (intervalMicroseconds), m_lastCheck (-1) { }

		void setInterval (uint32_t intervalSeconds)
		{
			m_intervalSeconds = intervalSeconds;
			m_intervalMicroseconds = 0;
		}
		void setInterval (uint32_t intervalSeconds, uint32_t intervalMicroseconds)
		{
			m_intervalSeconds = intervalSeconds;
			m_intervalMicroseconds = intervalMicroseconds;
		}

		bool process ()
		{
			struct timeval tv;
			::gettimeofday (&tv, NULL);
			uint64_t curTime = tv.tv_sec * 1000000 + tv.tv_usec;

			if (curTime - m_lastCheck > m_intervalSeconds * 1000000 + m_intervalMicroseconds)
			{
				m_lastCheck = curTime;
				return true;
			}
			return false;
		}

	private:
		uint32_t m_intervalSeconds, m_intervalMicroseconds;

		uint64_t m_lastCheck;
};

#endif
