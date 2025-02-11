#include "Server.hpp"

static int	setSocket(void)
{
	int server_fd = socket(PF_INET, SOCK_STREAM, 0);    //PF_NET = IPv4 ou IPv6; SOCK_STREAM 0 = TCP;
	if (server_fd < -1)
		throw std::runtime_error("Failed to create socket");
	return (server_fd);
}

static void	setNonBlocking(int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)   // accept, recv, or send infinite wait on blocking mode FLAGS***
		throw std::runtime_error("Failed to set non-blocking");
}

static void	setOpts(int sock)
{
	int opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // Port can be reused, when testing is good so it doesnt reopen
		throw std::runtime_error("Failed to set socket options");
}

static void	setBind(struct sockaddr_in _server_addr, int sock, int port)
{
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
		throw std::runtime_error("Failed to bind to that port");
}

static void setListen(int sock)
{
	if (listen(sock, MAX_CLIENTS) < 0)
		throw std::runtime_error("Failed to listen");
}

Server::Server(int port, const std::string &password) : _password(password)
{
	_server_fd = setSocket();
	setNonBlocking(_server_fd);
	setOpts(_server_fd);
	setBind(_server_addr, _server_fd, port);
	setListen(_server_fd);

	char portString[6];
	sprintf(portString, "%d", port);
	PRINT_COLOR(CYAN, "IRC Server listening on port: " + (std::string)portString);
}

Server::~Server( void ){
	close(_server_fd);
}

void Server::run( void )
{
	/*FD_ZERO(&this->fdList);
	FD_ZERO(&this->fdWrite);
	FD_SET(this->serverSocket, &this->fdList);
	while (1)
	{
		this->fdWrite = this->fdRead = this->fdList;
	select(this->maxFds + 1, &this->fdRead, &this->fdWrite, NULL, NULL);

		if (FD_ISSET(this->serverSocket, &this->fdRead))
			this->acceptClient();
		else 
			iterateClients();
	}


	FD_ZERO(&_read_fds);	// Init sockets
	FD_SET(_server_fd, &_read_fds);

	while (true)
	{
		_temp_fds = _read_fds;

		if (select(_server_fd + 1, &_temp_fds, NULL, NULL, NULL) < 0) // o fd mais alto e o do server
			throw std::runtime_error("Error: Select failed");
		for (int i = 0; i < _server_fd; i++)
		{
			if (FD_ISSET(i, &_temp_fds))
			{
				if (i == _server_fd)
					this->acceptConnection();	// New client
				else
					this->handleClient(i);		// Existing client
			}
		}
	}*/

	FD_ZERO(&_read_fds);			// Init sockets
	FD_SET(_server_fd, &_read_fds);

	while (true)
	{
		_temp_fds = _read_fds;

		// Mais eficiente se procurar o _server_fd mais alto ate agora e fizer loop so ate ai***
		if (select(FD_SETSIZE + 1, &_temp_fds, NULL, NULL, NULL) < 0)
			throw std::runtime_error("Select failed");
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_temp_fds))
			{
				if (i == _server_fd)
					this->acceptConnection();	// New client
				else
					this->handleClient(i);		// Existing client
			}
		}
	}
}

void Server::acceptConnection()
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int new_client_sock = accept(_server_fd, (struct sockaddr*)&client_addr, &addr_len);

	if (new_client_sock >= 0)
	{
		setNonBlocking(new_client_sock);
		FD_SET(new_client_sock, &_read_fds);
		_client_buffers[new_client_sock] = "";

		Client* newClient = new Client(new_client_sock);
		_clients[new_client_sock] = newClient;
		
		PRINT_COLOR(GREEN, "Client connected successfully: " << new_client_sock);

		//// WHY FD_SET twice?

		FD_SET(new_client_sock, &_read_fds);
		std::string welcome = "Please provide connection password using PASS command\r\n";
		send(new_client_sock, welcome.c_str(), welcome.length(), 0);
	}
	else
		throw std::runtime_error("Failed to create socket");
}

void Server::handleClient(int client_sock)
{
	char buff[BUFFER_SIZE];
	int bytes_read = recv(client_sock, buff, sizeof(buff) - 1, 0);
	
	if (bytes_read <= 0)
		clientDisconected(client_sock);
	else
		clientHandleMessage(client_sock, buff, bytes_read);
}

void Server::clientDisconected(int client_sock)
{
	if (DEBUG == DEBUG_ON)
		PRINT_COLOR(YELLOW, "Client disconnected: " << client_sock);
	close(client_sock);
	FD_CLR(client_sock, &_read_fds);
	_client_buffers.erase(client_sock);
}

void Server::clientHandleMessage(int client_sock, char *buff, int bytes_read)
{
	buff[bytes_read] = '\0';

	Client* client = _clients[client_sock];
	if (client->isAuthenticated())
		CommandHandler::processCommand(this, client, std::string(buff));
	else
		CommandHandler::processAuth(this, client, std::string(buff));
}

std::string	Server::getPassword(void)	const
{
	return _password;
}

Client*	Server::findClient(const std::string& clientName)
{
	std::map<int, Client*>::iterator it = this->_clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNickname() == clientName)
			return it->second;
		it++;
	}
	return NULL;
}

void Server::removeClient(int clientFd)
{
	std::map<int, Client*>::iterator it = _clients.find(clientFd);

	if (it != _clients.end())
	{
		Client* client = it->second;

		close(client->getSocketFd());
		delete client;
		_clients.erase(it);

		PRINT_COLOR(GREEN, "Client removed from server: FD " << clientFd);
	}
}

void Server::checkChannelName(const std::string& channelName, Client* client)
{
	char forbiddenChars[4] = {",\x07 "};

	if (channelName.empty() || std::strpbrk(channelName.c_str(), forbiddenChars) != NULL || channelName[0] != '#')
	{
		client->sendMessage(handleError(476));
		PRINT_ERROR(RED, "ERROR: Invalid channel name: " << channelName);
		throw (479);
	}
}

Channel* Server::createChannel(const std::string& channelName, Client* client)
{
	Channel *new_channel = new Channel(channelName);
	new_channel->addMember(client);
	new_channel->addOperator(client);
	this->_channels[channelName] = new_channel;
	
	PRINT_COLOR(CYAN, "Server: Channel " << channelName << " created by " << client->getUsername() << "!");
	return new_channel; // CREATE CHANNEL IF IT DOESN'T EXIST
}

Channel* Server::findChannel(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator it = this->_channels.find(channelName);
	if (it != _channels.end())
		return it->second; // RETURN CHANNEL IF EXISTS
	return NULL;
}

int Server::isNicknameInUse(std::string nickname)
{
    for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
    {
        if (it->second == NULL)
            continue;

        if (it->second->getNickname() == nickname)
            return true;
    }
    return false;
}
