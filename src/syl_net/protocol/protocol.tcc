#include "protocol.h"

namespace SylNet
{
	template<class T>
	bool Protocol::registerNetworkable(
		Networkable::Id _id, T& _object, 
		SylUtil::InterpolationFunction<T> _interpolationFunction, bool _disableDefaultInterpolation)
	{
		NetworkableTarget<T>* target = new NetworkableTarget<T>(
			_object, _interpolationFunction, _disableDefaultInterpolation);
		auto pair = std::make_pair(_id, target);
		auto result = m_networkables.insert(pair);

		return result.second;
	}
};