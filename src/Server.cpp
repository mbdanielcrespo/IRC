#include <Server.hpp>


static int	setSocket(void)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);    //AF_NET = IPv4; SOCK_STREAM 0 = TCP;
	if (server_fd < 0)
		throw std::runtime_error("Failed to create socket");
	return (server_fd);
}

static void	setNonBlocking(int sock)
{
	int flags = fcntl(sock, F_SETFL, O_NONBLOCK);   // accept, recv, or send infinite wait on blocking mode FLAGS***
	if (flags < 0)
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
		throw std::runtime_error("Failed to bind");
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
	
	if (DEBUG == DEBUG_ON)
		PRINT_COLOR(CYAN, "IRC Server listening on port ... " << port);
}

Server::~Server()
{
	close(_server_fd);
}

void Server::run()
{

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
		
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(GREEN, "Client connected successfully!");

		FD_SET(new_client_sock, &_read_fds);
		std::string welcome = "Please provide connection password using PASS command\r\n";
		send(new_client_sock, welcome.c_str(), welcome.length(), 0);
	}

	else
		throw std::runtime_error("Failed to create socket");
}

void	Server::handleClient(int clinet_sock)
{
	char buff[BUFFER_SIZE];
	int bytes_read = recv(clinet_sock, buff, sizeof(buff) - 1, 0);

	if (bytes_read <= 0)
		clientDisconected(clinet_sock);
	else
		clientHandleMessage(clinet_sock, buff, bytes_read);
}

void	Server::clientDisconected(int i)
{
	if (DEBUG == DEBUG_ON)
		PRINT_COLOR(YELLOW, "Client disconnected: " << i);
	close(i);
	FD_CLR(i, &_read_fds);
	_client_buffers.erase(i);
}

void	Server::clientHandleMessage(int clinet_sock, char *buff, int bytes_read)
{
	buff[bytes_read] = '\0';

	Client* client = _clients[clinet_sock];
	CommandHandler::processCommand(this, client, std::string(buff));
}

std::string	Server::getPassword(void)	const
{
	return _password;
}

Client*		Server::findClient(const std::string& clientName)
{
	PRINT_COLOR(YELLOW, "Finding clinet ...");
	(void)clientName;
	return NULL;
}

Channel*	Server::findChannel(const std::string& channelName)
{
	PRINT_COLOR(YELLOW, "Finding channel ...");
	(void)channelName;
	return NULL;
}