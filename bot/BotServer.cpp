#include "BotServer.hpp"
#include "BotSocket.hpp"

void BotServer::run()
{
	while (true)
	{
		int ready = poll(&_pollFds[0], _pollFds.size(), 5000); // 5 seconds poll
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Poll error");
		}

		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == )
					acceptNewClient();
				else
					handleClientData(_pollFds[i].fd);
			}
		}
	}
}
