///////////////////////////////////////
//  (             (                
//  )\ )     (    )\ )             
// (()/((    )\  (()/(    (   )    
//  /(_))\ )((_)  /(_))  ))\ /((   
// (_))(()/( _   (_))_  /((_|_))\  
// / __|)(_)) |   |   \(_)) _)((_) 
// \__ \ || | |   | |) / -_)\ V /  
// |___/\_, |_|   |___/\___| \_/   
//      |__/                       
//
// 2014 Moritz Kretz
///////////////////////////////////////

#ifndef SYLUTIL_TIME_H
#define SYLUTIL_TIME_H

#include <chrono>

namespace SylUtil
{
	typedef std::chrono::milliseconds::rep time_ms;

	inline time_ms getCurrentTimeMs()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	inline time_ms convertSecondsToMs(float _seconds)
	{
		if (_seconds < 0.0f)
		{
			_seconds = 0.0f;
		}

		return static_cast<time_ms>(_seconds * 1000.0f);
	}

	inline void initializeNextActivityTime(time_ms& _nextActivityTime, time_ms _delay)
	{
		time_ms currentTime = getCurrentTimeMs();

		if (_nextActivityTime > currentTime && _nextActivityTime - currentTime > _delay)
		{
			_nextActivityTime = currentTime + _delay;
		}
	}
};

#endif