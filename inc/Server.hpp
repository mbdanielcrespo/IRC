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
#include <errno.h>

#include <cstdlib>
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
		int										_server_fd;
		fd_set									_read_fds, _temp_fds;
		static std::string						_pass;
		struct sockaddr_in						_server_addr;
		std::map<int, Client*> 					_clients;
		std::map<int, std::string>				_client_buffers;
		std::map<std::string, Channel*>			_channels;

	public:
		std::map< std::string, void(Server::*)(Client* client, const std::vector<std::string>& params) > commands;
		std::map< std::string, void(Server::*)(Client* client, const std::vector<std::string>& params) > authCommands;

		Server(int port, const std::string& passw);
		~Server(void);

	void		run(void);
	void		acceptConnection(void);
	void		handleClient(int client_sock);
	void		clientDisconected(int client_sock);
	void		clientHandleMessage(int client_sock, char *buff, int bytes_read);

	const std::string&	getPass(void) const;
	fd_set&		getReadFds(void);

	Client*		findClient(const std::string& clientName);
	Channel*	findChannel(const std::string& channelName);

	void		removeClient(int clientFd);
	void		checkChannelName(const std::string& channelName, Client* client);
	Channel*	createChannel(const std::string& channelName, Client* client);
	int 		isNicknameInUse(std::string);
	void		checkChannel(Channel *channel);
	void		checkClient(Client *client);

	void		handlePass(Client* client, const std::vector<std::string>& params);
	void		handleNick(Client* client, const std::vector<std::string>& params);
	void		handleUser(Client* client, const std::vector<std::string>& params);
	void		handlePrivMsg(Client* client, const std::vector<std::string>& params);
	void		handleJoin(Client* client, const std::vector<std::string>& params);
	void		handlePart(Client* client, const std::vector<std::string>& params);
	void		handleQuit(Client* client, const std::vector<std::string>& params);
	void		handleKick(Client* client, const std::vector<std::string>& params);
	void		handleInvite(Client* client, const std::vector<std::string>& params);
	void		handleTopic(Client* client, const std::vector<std::string>& params);
	void		handleMode(Client* client, const std::vector<std::string>& params);
};
