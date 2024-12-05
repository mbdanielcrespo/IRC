#include <Client.hpp>

Client::Client(int socket_fd) : 
	_nickname(""),
	_username(""),
	_hostname(""),
	_realname(""),
	_socketFd(socket_fd),
	_hasNickname(false),
	_hasUsername(false),
	_isAuthenticated(false),
	_isOperator(false),
	_connectionTime(time(NULL)) {
	if (_socketFd < 0) {
		throw std::invalid_argument("Invalid socket file descriptor");
	}
}

Client::~Client()
{
	_joinedChannels.clear();
}

bool Client::authenticate(const std::string& password, const std::string& srv_pass)
{
	if (!password.empty() && (password == srv_pass) && _hasNickname && _hasUsername)
	{
		_isAuthenticated = true;
		return true;
	}
	return false;
}

void Client::setNickname(const std::string& nickname)
{
	if (!nickname.empty())
	{
		_nickname = nickname;
		_hasNickname = true;
		PRINT_COLOR(CYAN, "NICK successfully set to: " << nickname) << "!";
	}
}

void Client::setUsername(const std::string& username, const std::string& realname)
{
	if (!username.empty())
	{
		_username = username;
		_realname = realname;
		_hasUsername = true;
		PRINT_COLOR(CYAN, "USER successfully set to: " << username) << "!";
	}
}

void Client::joinChannel(Channel* channel)
{
	if (channel != NULL)
	{
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(CYAN, "ClientSide: " << this->getUsername() << " joined " << channel->getName());
		channel->addMember(this),
		_joinedChannels[channel->getName()] = channel;
	}
}

void Client::leaveChannel(const std::string& channel_name)
{
	std::map<std::string, Channel*>::iterator it = _joinedChannels.find(channel_name);
	if (it != _joinedChannels.end())
	{
		// Access channel and remove clinet
		_joinedChannels.erase(it);
	}
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
		// Actual socket send logic would be implemented here
		// For example: 
		// send(_socketFd, message.c_str(), message.length(), 0);
	}
}

void Client::sendPrivateMessage(Client* recipient, const std::string& message) {
	// Validate recipient and message
	if (recipient != NULL && !message.empty()) {
		// In a real IRC implementation, this would:
		// 1. Format the message according to IRC protocol
		// 2. Use socket communication to send the message
		recipient->sendMessage(message);
	}
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

bool Client::isOperator() const
{
	return _isOperator; 
}

std::string Client::getPrefix() const
{
	return _nickname + "!" + _username + "@" + _hostname;
}

bool Client::isInChannel(const std::string& channel_name) const
{
	return _joinedChannels.find(channel_name) != _joinedChannels.end();
}

bool Client::isAuthenticated() const
{
	return _isAuthenticated;
}