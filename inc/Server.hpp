#pragma once

#include <map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>

#include <cstdlib>
#include <stdexcept>
#include <algorithm>

#include "main.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"

# define MAX_CLIENTS 100
# define BUFFER_SIZE 1024

class Server
{
	private:
		int								_server_fd;
		fd_set							_read_fds, _temp_fds;
		std::string						_password;
		struct sockaddr_in				_server_addr;
		std::map<int, Client*> 			_clients;
		std::map<int, std::string>		_client_buffers;
		std::map<std::string, Channel*>	_channels;
		

	public:
		Server(int port, const std::string &passw);
		~Server(void);

		void run(void);
		void acceptConnection();
		void handleClient(int client_sock);
		void clientDisconected(int client_sock);
		void clientHandleMessage(int client_sock, char *buff, int bytes_read);

		std::string	getPassword(void)	const;
		Client*		findClient(const std::string& clientName);
		void		removeClient(int clientFd);
		Channel*	findChannel(const std::string& channelName, Client* client);
		void		checkChannelName(const std::string& channelName, Client* client);
		Channel*	createChannel(const std::string& channelName, Client* client);
		int 		isNicknameInUse(std::string);
};
