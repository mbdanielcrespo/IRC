#include <Client.hpp>

Client::Client(int socket_fd) : 
	_nickname(""),
	_username(""),
	_hostname(""),
	_id(""),
	//_realname(""),
	_socketFd(socket_fd),
	_hasNickname(false),
	_hasUsername(false),
	_isAuthenticated(false),
	_isOperator(false),
	_connectionTime(time(NULL))
{
	if (_socketFd < 0)
		throw std::invalid_argument("Invalid socket file descriptor");
}

void Client::resolveHostname(int _socketFd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	if (getpeername(_socketFd, (struct sockaddr*)&addr, &addr_len) == -1)
	{
		PRINT_ERROR(RED, "ERROR: Failed to get host name!");
		return;
	}

	const char* ip = inet_ntoa(addr.sin_addr);

	struct hostent* host_entry = gethostbyname(ip);
	if (host_entry && host_entry->h_name)
	{
		this->setHostname(host_entry->h_name);
		PRINT_COLOR(YELLOW, "Hostname resolved to: " << _hostname);
	}
	else
	{
		this->setHostname(ip);
		PRINT_COLOR(YELLOW, "Using IP as hostname: " << _hostname);
	}
}

Client::~Client()
{
	_joinedChannels.clear();
}

bool Client::authenticate(const std::string& password, const std::string& srv_pass)
{
	if (password.empty())
		throw(461);
	if (password != srv_pass)
		throw(464);
	if (_hasNickname && _hasUsername)
	{
		_isAuthenticated = true;
		this->resolveHostname(_socketFd);
		this->setId();
		this->sendMessage("PASS successfully validated!\n");
		return true;
	}
	PRINT_COLOR(RED, "PASS incorrect or NICK/USER unset!");
	return false;
}

void Client::joinChannel(Channel* channel)
{
	if (channel != NULL && !channel->isMember(this->getNickname()))
	{
		this->sendMessage(":" + this->getId() + " JOIN :" + channel->getName() + "\r\n");
		channel->addMember(this),
		_joinedChannels[channel->getName()] = channel;
	}
}

void Client::leaveChannel(const std::string& channel_name)
{
	std::map<std::string, Channel*>::iterator it = _joinedChannels.find(channel_name);
	if (it != _joinedChannels.end())
		_joinedChannels.erase(it);
}

void Client::setChannelOperatorStatus(const std::string& channel_name, bool is_op)
{
	if (isInChannel(channel_name))
		_isOperator = is_op;
}

void Client::sendMessage(const std::string& message)
{
	if (_socketFd >= 0 && !message.empty())
	{
		std::string new_mesage = message + "\r\n";
		ssize_t bytes_sent = send(_socketFd, new_mesage.c_str(), message.length(), 0);
		PRINT_COLOR(CYAN, "Message sent to client " + this->getNickname() + ": " + message);
		if (bytes_sent == -1)
			PRINT_ERROR(RED, "ERROR: Sending message to client " << _socketFd << "!");
	}
	else
		PRINT_ERROR(RED, "ERROR: Invalid socket or empty message!");
}

void Client::sendPrivateMessage(Client* recipient, const std::string& message)
{
	if (recipient != NULL && !message.empty())
		recipient->sendMessage("Private Message recived: " + message + "\r\n");
	else
	{
		if (DEBUG == DEBUG_ON)
			PRINT_ERROR(RED, "ERROR: Something went wrong when sending private message! \r\n");
	}
}

void Client::setNickname(const std::string& nickname)
{
	if (!nickname.empty())
	{
		_nickname = nickname;
		_hasNickname = true;
		this->sendMessage("NICK successfully set to: " + nickname + "!\r\n");
		if (_hasUsername)
			this->sendMessage(":server 001 " + _nickname + " :Welcome to the IRC network!\r\n");
	}
}

void Client::setUsername(const std::string& username)
{
	if (!username.empty())
	{
		_username = username;
		_hasUsername = true;
		this->sendMessage("USER successfully set to: " + username + "!\r\n");
		if (_hasNickname)
			this->sendMessage(":server 001 " + _nickname + " :Welcome to the IRC network!\r\n");
	}
}

void Client::setHostname(const std::string& hostname)
{
	if (!hostname.empty())
	{
		_hostname = hostname;
		PRINT_COLOR(CYAN, "HOSTNAME successfully set to: " << hostname << "!");
	}
}

void Client::setId()
{
	if (_username != "" && _hostname != "" && _nickname != "")
	{
		_id = _nickname + "!" + _username + "@" + _hostname;
		PRINT_COLOR(CYAN, "ID succsessfully set to: " << _id << "!");
	}
	else
		PRINT_COLOR(YELLOW, "Unable to set id! ... USER: " + _username + " | HOST: " + _hostname + " | NICK: " + _nickname);

}

std::string Client::getNickname() const
{
	return _nickname; 
}

std::string Client::getUsername() const
{
	return _username; 
}

int Client::getSocketFd() const
{
	return _socketFd; 
}

std::map<std::string, Channel*> Client::getJoinedChannels() const
{
    return _joinedChannels;
}

bool Client::isOperator() const
{
	return _isOperator; 
}

std::string Client::getId() const
{
	return _id;
}

bool Client::isInChannel(const std::string& channel_name) const
{
	return _joinedChannels.find(channel_name) != _joinedChannels.end();
}

bool Client::isAuthenticated() const
{
	return _isAuthenticated;
}
