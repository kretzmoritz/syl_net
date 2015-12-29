#include <iostream>

#include "protocol/tcp_server.h"
#include "protocol/tcp_client.h"

#include "featured/broadcaster.h"
#include "featured/browser.h"
#include "net/connection.h"

#include <array>

int main()
{
	int i;
	std::cin >> i;

	if (i == 0)
	{
		unsigned short j;
		std::cin >> j;

		std::string str;
		std::cin >> str;

		SylNet::TcpServer server;
		SylNet::Broadcaster bc(str, j, 3000, 1.0f);

		std::vector<int> c;
		float f;
		std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> m;
		std::array<int, 2> a;

		server.registerNetworkable(0, c);
		server.registerNetworkable(1, f);
		server.registerNetworkable(2, m);
		server.registerNetworkable(3, a);
		server.listen_start(j, 0);
		server.setConnectionTimeout(0.0f);
		
		while (true)
		{
			server.accept();
			server.receive();
			bc.send();

			if (!m.empty())
			{
				auto vec = m.begin()->second;
				for (auto i = vec.begin(); i != vec.end(); ++i)
				{
					std::cout << i->second << std::endl;
				}
			}
		}
	}
	else
	{
		SylNet::TcpClient client;
		SylNet::Browser browser(3000, 5.0f);

		std::vector<int> c;
		c.push_back(1);
		float f = 0.5f;
		std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> m;
		m.emplace(std::make_pair(1, 1), std::vector<std::pair<int, int>>{std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3)});
		std::array<int, 2> a;
		a[0] = 1;
		a[1] = 2;

		client.registerNetworkable(0, c);
		client.registerNetworkable(1, f);
		client.registerNetworkable(2, m);
		client.registerNetworkable(3, a);
		client.open("127.0.0.1", 5000);
		client.setConnectionTimeout(0.0f);

		while (true)
		{
			browser.receive();
			std::vector<std::pair<std::string, SylNet::Connection>> bla;
			browser.getRemoteServers(bla);
			
			for (auto i = bla.begin(); i != bla.end(); ++i)
			{
				std::cout << i->first << ' ' << i->second.address << ' ' << i->second.port << std::endl;
			}

			if (client.send())
			{
				f += 0.1f;
			}
		}
	}

	std::cout << "Hello world." << std::endl;
	std::getchar();

	return 0;
}