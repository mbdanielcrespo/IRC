#include <Server.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Server::Server(int port, const std::string &password) : _password(password)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);    //AF_NET = IPv4; SOCK_STREAM 0 = TCP;
	if (_server_fd < 0)
		throw std::runtime_error("Failed to create socket");

	int flags = fcntl(_server_fd, F_SETFL, O_NONBLOCK)   // accept, recv, or send infinite wait on blocking mode FLAGS***
	if (flags < 0)
		throw std::runtime_error("Failed to set non-blocking");

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // Port can be reused, when testing is good so it doesnt reopen
		throw std::runtime_error("Failed to set socket options");

	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(port);

	if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
		throw std::runtime_error("Failed to bind");

	if (listen(_server_fd, MAX_CLIENTS) < 0)
		throw std::runtime_error("Failed to listen");

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		_fds[i].fd = -1;
		_fds[i].events = POLLIN;
	}
	_fds[0].fd = server_fd;
	_fds[0].events = POLLIN;
	std::cout << "IRC Server listening on port " << port << std::endl;

	if (DEBUG == 1)
	{
		std::cout << "--------------------------------------------------------\n";
		std::cout << "Server fd:	" << _server_fd << "\n";
		std::cout << "Flags:	" << flags << "\n";
		std::cout << "Port options:	" << opt << "\n";
		std::cout << "Server family:	" << _server_addr.sin_family << "\n";
		std::cout << "Server address:	" << _server_addr.sin_addr.s_addr << "\n";
		std::cout << "Server fd:	" << _server_fd << "\n";
		std::cout << "Server fd:	" << _server_fd << "\n";
		for (int x : _fds) std::cout << x.fd << " ";
		for (int x : _fds) std::cout << x.events << " ";
	}
}

Server::~Server()
{
	close(server_fd);
}

void Server::run()
{
	while (true)
	{
		int poll_count = poll(fds, MAX_CLIENTS, -1);
		if (poll_count < 0)
		{
			std::cerr << "Poll error: " << strerror(errno) << std::endl;
			break;
		}

		if (fds[0].revents & POLLIN)
		{
			acceptConnection();
		}

		for (int i = 1; i < MAX_CLIENTS; ++i)
		{
			if (fds[i].fd != -1 && (fds[i].revents & POLLIN))
			{
				handleClient(i);
			}
		}
	}
}

void Server::acceptConnection()
{
	 struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		if (errno != EWOULDBLOCK) {
			std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
		}
		return;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
		std::cerr << "Failed to set client socket non-blocking: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	Client *new_client = new Client(client_fd);
	clients.push_back(new_client);
	std::cout << "New client connected." << std::endl;

	for (int i = 1; i < MAX_CLIENTS; ++i)
	{
		if (fds[i].fd == -1)
		{
			fds[i].fd = client_fd;
			fds[i].events = POLLIN;
			return;
		}
	}
}

void Server::handleClient(int i)
{
	char buffer[BUFFER_SIZE];
	int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received <= 0) {
		if (bytes_received == 0) {
			std::cout << "Client disconnected." << std::endl;
		} else {
			std::cerr << "Recv error: " << strerror(errno) << std::endl;
		}
		close(fds[i].fd);
		fds[i].fd = -1;
		return;
	}

	buffer[bytes_received] = '\0';
	std::string message(buffer);

	// Find the associated client
	Client *client = nullptr;
	for (size_t j = 0; j < clients.size(); ++j) {
		if (clients[j]->getSocket() == fds[i].fd) {
			client = clients[j];
			break;
		}
	}

	if (client) {
		parseCommand(client, message);
	}
}

void Server::parseCommand(Client *client, const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "NICK") {
        std::string nickname;
        iss >> nickname;
        client->setNickname(nickname);
        std::cout << "Set nickname: " << nickname << std::endl;
    } else if (cmd == "USER") {
        std::string username;
        iss >> username;
        client->setUsername(username);
        std::cout << "Set username: " << username << std::endl;
        if (!client->isAuthenticated()) {
            client->authenticate();
            std::cout << "Client authenticated." << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << cmd << std::endl;
    }
}