#pragma once
#include <main.hpp>
#include <Channel.hpp>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <stdexcept>
#include <iostream>

class Channel;

class Client
{
	private:
		std::string     _nickname;
		std::string     _username;
		std::string     _hostname;
		std::string     _realname;
		int             _socketFd;

		bool            _isAuthenticated;
		bool            _isRegistered;
		
		std::map<std::string, Channel*> _joinedChannels;
		bool            _isOperator;

		time_t          _connectionTime;

	public:
		Client(int socket_fd);
		~Client();

		bool    authenticate(const std::string& password, const std::string& srv_pass);
		void    setNickname(const std::string& nickname);
		void    setUsername(const std::string& username, const std::string& realname);
		bool    isRegistered() const;

		// Channel interaction methods
		void    joinChannel(Channel* channel);
		void    leaveChannel(const std::string& channel_name);
		bool    isInChannel(const std::string& channel_name) const;
		void    setChannelOperatorStatus(const std::string& channel_name, bool is_op);

		// Messaging methods
		void    sendMessage(const std::string& message);
		void    sendPrivateMessage(Client* recipient, const std::string& message);

		// Getters
		std::string getNickname() const;
		std::string getUsername() const;
		int         getSocketFd() const;
		bool        isOperator() const;

		// Utility methods
		std::string getPrefix() const; // Returns nickname!username@hostname
};