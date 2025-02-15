#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#include <Server.hpp>
#include <Client.hpp>

class Server;
class Client;

typedef std::map<std::string, void(Server::*)(Client* client, const std::vector<std::string>& params)>::iterator commandIt;
typedef std::map<std::string, void(Server::*)(Client* client, const std::vector<std::string>& params)> CommandMap;

class CommandHandler
{
	private:
		std::string _command;
		std::string _raw_command;
		std::vector<std::string> _params;
		std::vector<std::string> _arguments;
		std::vector<std::string> _received_cmds;

	public:
		CommandHandler(std::string raw_cmd);

	void	processCommand(Server* server, Client* client, CommandMap map, int errorNum);
	void	processAuth(Server* server, Client* client);
};
