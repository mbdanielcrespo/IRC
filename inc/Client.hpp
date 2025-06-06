#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <stdexcept>
#include <iostream>

#include <main.hpp>
#include <Channel.hpp>

class Channel;

class Client
{
	private:
		std::string		_nickname;
		std::string		_username;
		std::string		_hostname;
		std::string		_id;
		int				_socketFd;

		bool			_hasNickname;
		bool			_hasUsername;

		bool			_isAuthenticated;
		bool			_isOperator;
		
		time_t			_connectionTime;

		std::map<std::string, Channel*>	_joinedChannels;

	public:
		Client(int socket_fd);
		~Client();

		bool	authenticate(const std::string& password, const std::string& srv_pass);
		void	setNickname(const std::string& nickname);
		void	setUsername(const std::string& username);
		void	setHostname(const std::string& hostname);
		void	setId();
		bool	isAuthenticated() const;

		void	joinChannel(Channel* channel);
		void	leaveChannel(const std::string& channel_name);
		bool	isInChannel(const std::string& channel_name) const;
		void	setChannelOperatorStatus(const std::string& channel_name, bool is_op);
		void	sendChannelModes(Channel *channel);

		void	sendMessage(const std::string& message);
		void	sendPrivateMessage(Client* recipient, const std::string& message);

		std::string	getNickname() const;
		std::string	getUsername() const;
		std::string getId() const;
		std::map<std::string, Channel*> getJoinedChannels() const;

		int			getSocketFd() const;
		bool		isOperator() const;

		void		resolveHostname(int _socketFd);
};