#include <array>

#include "serialization.h"
#include "../../syl_util/stl/stl.h"

namespace SylNet
{
	template<class T>
	sf::Packet& write(sf::Packet& _packet, T const& _arg, long)
	{
		_packet.append(&_arg, sizeof(T));
		return _packet;
	}

	template<class T>
	auto write(sf::Packet& _packet, T const& _arg, int) -> decltype(_arg.begin(), _packet)
	{
		_packet << _arg.size();

		for (auto i = _arg.begin(); i != _arg.end(); ++i)
		{
			_packet << *i;
		}

		return _packet;
	}

	template<class T>
	sf::Packet& operator<<(sf::Packet& _packet, T const& _arg)
	{
		return write(_packet, _arg, 0);
	}

	template<class T>
	sf::Packet& read(sf::Packet& _packet, T& _arg, long)
	{
		if (sf::checkSize(_packet, sizeof(T)))
		{
			_arg = *reinterpret_cast<T const*>(&static_cast<char const*>(_packet.getData())[sf::getReadPos(_packet)]);
			sf::moveReadPos(_packet, sizeof(T));
		}

		return _packet;
	}

	template<class T, size_t size>
	void update_container(sf::Packet& _packet, std::array<T, size>& _arg, sf::Uint32 _receivedElements, int)
	{
		for (sf::Uint32 i = 0; i < _receivedElements; ++i)
		{
			if (i < size)
			{
				_packet >> _arg[i];
				continue;
			}

			skip(_packet, T());
		}
	}

	template<class T>
	auto update_container(sf::Packet& _packet, T& _arg, sf::Uint32 _receivedElements, long) -> decltype(_arg.resize(0), void())
	{
		if (_arg.size() != _receivedElements)
		{
			_arg.resize(_receivedElements);
		}

		for (auto i = _arg.begin(); i != _arg.end(); ++i)
		{
			_packet >> *i;
		}
	}

	template<class T>
	auto update_container(sf::Packet& _packet, T& _arg, sf::Uint32 _receivedElements, int) -> decltype(_arg.key_comp(), void())
	{
		_arg.clear();

		for (sf::Uint32 i = 0; i < _receivedElements; ++i)
		{
			SylUtil::RemoveConst<T::value_type>::type temp;
			_packet >> temp;

			_arg.emplace(temp);
		}
	}

	template<class T>
	auto read(sf::Packet& _packet, T& _arg, int) -> decltype(_arg.begin(), _packet)
	{
		sf::Uint32 receivedElements;
		_packet >> receivedElements;

		update_container(_packet, _arg, receivedElements, 0);

		return _packet;
	}

	template<class T>
	sf::Packet& operator>>(sf::Packet& _packet, T& _arg)
	{
		return read(_packet, _arg, 0);
	}

	template<class T>
	void skip_implementation(sf::Packet& _packet, T const& _arg, long)
	{
		sf::moveReadPos(_packet, sizeof(T));
	}

	template<class T>
	auto skip_implementation(sf::Packet& _packet, T const& _arg, int) -> decltype(_arg.begin(), void())
	{
		sf::Uint32 receivedElements;
		_packet >> receivedElements;

		for (sf::Uint32 i = 0; i < receivedElements; ++i)
		{
			skip(_packet, T::value_type());
		}
	}

	template<class T>
	void skip(sf::Packet& _packet, T const& _arg)
	{
		skip_implementation(_packet, _arg, 0);
	}

	template<class T, class U>
	sf::Packet& operator<<(sf::Packet& _packet, std::pair<T, U> const& _arg)
	{
		_packet << _arg.first;
		_packet << _arg.second;

		return _packet;
	}

	template<class T, class U>
	sf::Packet& operator>>(sf::Packet& _packet, std::pair<T, U>& _arg)
	{
		_packet >> _arg.first;
		_packet >> _arg.second;

		return _packet;
	}

	template<class T, class U>
	void skip(sf::Packet& _packet, std::pair<T, U> const& _arg)
	{
		skip(_packet, _arg.first);
		skip(_packet, _arg.second);
	}

	template<class T>
	void serialize(
		T& _object, SerializationContext const& _ctx,
		SylUtil::InterpolationFunction<T> const& _interpolationFunction)
	{
		switch (_ctx.state)
		{
		case Networkable::State::READ:
			if (!_interpolationFunction)
			{
				_ctx.packet >> _object;
			}
			else
			{
				skip(_ctx.packet, _object);
			}
			break;
		case Networkable::State::WRITE:
		case Networkable::State::WRITE_LOCAL:
			_ctx.packet << _object;
			break;
		case Networkable::State::INTERPOLATE:
			if (_interpolationFunction)
			{
				T prev;
				_ctx.prevPacket >> prev;

				T current;
				_ctx.packet >> current;

				_interpolationFunction(_object, prev, current, _ctx.interpolationProgress);
			}
			else
			{
				skip(_ctx.prevPacket, _object);
				skip(_ctx.packet, _object);
			}
			break;
		}
	}
};