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

#include <string>
#include <netinet/in.h>
#include <poll.h>
#include <main.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

class Server
{
	private:
		int					_server_fd;
		std::string			_password;
		struct sockaddr_in	_server_addr;
		struct pollfd		_fds[MAX_CLIENTS];
		std::vector<Client*> _clients;

	public:
		Server(int port, const std::string &passw);
		~Server();
		void run();
		void handleClient(int i); // We will modify this

	private: 
		void acceptConnection();
		void handleClient(int i);
		void parseCommand(Client *client, const std::string &command);
};
