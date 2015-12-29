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

#ifndef SYLUTIL_INTERPOLATION_H
#define SYLUTIL_INTERPOLATION_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>

namespace SylUtil
{
	//args: result, previous value, current value, interpolation progress
	template<class T>
	using InterpolationFunction = std::function<void(T&, T const&, T const&, float)>;

	template<class T>
	void linear_interpolation(T& _result, T const& _previousValue, T const& _currentValue, float _interpolationProgress)
	{
		_result = _previousValue + static_cast<T>((_currentValue - _previousValue) * _interpolationProgress);
	}

	template<class T>
	void cosine_interpolation(T& _result, T const& _previousValue, T const& _currentValue, float _interpolationProgress)
	{
		float interpolationProgress = (1.0f - cosf(_interpolationProgress * M_PI)) / 2.0f;
		linear_interpolation(_result, _previousValue, _currentValue, interpolationProgress);
	}
};

#endif