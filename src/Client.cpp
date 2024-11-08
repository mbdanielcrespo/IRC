#include "Client.hpp"
#include <unistd.h>

Client::Client(int socket_fd) : _socket_fd(socket_fd), _authenticated(false) {}

Client::~Client()
{
	if (socket_fd >= 0)
		close(socket_fd);
}

void Client::setNickname(const std::string &nickname) { this->_nickname = nickname; }
void Client::setUsername(const std::string &username) { this->_username = username; }
void Client::authenticate() { _authenticated = true; }
int Client::getSocket() const { return _socket_fd; }
std::string Client::getNickname() const { return _nickname; }
std::string Client::getUsername() const { return _username; }
bool Client::isAuthenticated() const { return _authenticated; }
