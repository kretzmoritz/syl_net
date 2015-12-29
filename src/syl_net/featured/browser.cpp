#include "browser.h"

namespace SylNet
{
	Browser::Browser(unsigned short _browserPort, float _keepAliveSeconds)
	{
		m_udp.bind(_browserPort);
		m_udp.acceptPacketsFromUnknownConnections();
		m_udp.registerNetworkable(0, *this);

		setKeepAliveTime(_keepAliveSeconds);
	}

	void Browser::setBrowserPort(unsigned short _browserPort)
	{
		m_udp.unbind();
		m_udp.bind(_browserPort);
	}

	void Browser::setKeepAliveTime(float _seconds)
	{
		m_keepAliveTime = SylUtil::convertSecondsToMs(_seconds);
	}

	void Browser::getRemoteServers(std::vector<std::pair<std::string, Connection>>& _remoteServers)
	{
		_remoteServers.clear();

		for (auto i = m_remoteServers.begin(); i != m_remoteServers.end(); ++i)
		{
			std::string serverName = i->second.first;
			Connection connection = i->first;

			_remoteServers.push_back(std::make_pair(serverName, connection));
		}
	}

	void Browser::receive()
	{
		m_udp.receive();

		for (auto i = m_remoteServers.begin(); i != m_remoteServers.end();)
		{
			SylUtil::time_ms removalTime = i->second.second;

			if (SylUtil::getCurrentTimeMs() < removalTime)
			{
				++i;
				continue;
			}

			i = m_remoteServers.erase(i);
		}
	}

	void serialize(
		Browser& _object, SerializationContext const& _ctx,
		SylUtil::InterpolationFunction<Browser> const& _interpolationFunction)
	{
		if (_ctx.state == Networkable::State::READ)
		{
			std::string serverName;
			Connection connection;

			serialize(serverName, _ctx);
			serialize(connection, _ctx);

			SylUtil::time_ms removalTime = SylUtil::getCurrentTimeMs() + _object.m_keepAliveTime;
			
			auto i = _object.m_remoteServers.find(connection);

			if (i != _object.m_remoteServers.end())
			{
				std::string& str = i->second.first;
				SylUtil::time_ms& t = i->second.second;

				str = serverName;
				t = removalTime;
			}
			else
			{
				auto pair0 = std::make_pair(serverName, removalTime);
				auto pair1 = std::make_pair(connection, pair0);

				_object.m_remoteServers.insert(pair1);
			}
		}
	}
};