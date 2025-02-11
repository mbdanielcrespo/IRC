/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 00:31:32 by marvin            #+#    #+#             */
/*   Updated: 2024/11/07 00:31:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <netinet/in.h>
#include <main.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <Client.hpp>
#include <Channel.hpp>
#include <CommandHandler.hpp>
#include <cstring>
#include <string>
#include <errno.h>


#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

class Server
{
	private:
		int								_server_fd;
		std::string						_password;
		struct sockaddr_in				_server_addr;
		fd_set							_read_fds, _temp_fds;
		std::map<int, std::string>		_client_buffers;
		std::map<int, Client*> 			_clients;
		std::map<std::string, Channel*>	_channels;
		
	public:
		Server(int port, const std::string &passw);
		~Server();
		void run();
		void acceptConnection();
		void handleClient(int client_sock);
		void clientDisconected(int client_sock);
		void clientHandleMessage(int client_sock, char *buff, int bytes_read);

		std::string	getPassword(void)	const;
		fd_set&		getReadFds(void);
		Client*		findClient(const std::string& clientName);
		void		removeClient(int clientFd);
		Channel*	findChannel(const std::string& channelName, Client* client);
		int 		isNicknameInUse(std::string);
};
