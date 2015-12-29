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

#ifndef SYLUTIL_STL_H
#define SYLUTIL_STL_H

namespace SylUtil
{
	template<class T>
	struct RemoveConst
	{
	public:
		typedef typename std::remove_const<T>::type type;
	};

	template<class T, class U>
	struct RemoveConst<std::pair<T, U>>
	{
	public:
		typedef typename std::pair<typename std::remove_const<T>::type, typename std::remove_const<U>::type> type;
	};
};

#endif