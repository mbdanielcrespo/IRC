#include <CommandHandler.hpp>
#include <Channel.hpp>

static std::vector<std::string> splitCommand(const std::string& raw_cmd)
{
	std::vector<std::string> parts;
	std::istringstream iss(raw_cmd);
	std::string part;

	while (iss >> part)
		parts.push_back(part);
	return parts;
}

CommandHandler::CommandHandler(std::string raw_cmd) : _raw_command(raw_cmd)
{
	if (_raw_command.empty())
		throw(1001);

	_arguments = splitCommand(_raw_command);
	if (_arguments.empty())
		throw (1001);

	_command = _arguments[0];
	std::transform(_command.begin(), _command.end(), _command.begin(), ::toupper);
	_params = std::vector<std::string>(_arguments.begin() + 1, _arguments.end());
}

void CommandHandler::processAuth(Server* server, Client* client)
{
	if (_raw_command.compare(0, 10, "CAP LS 302") == 0)
	{
		PRINT_COLOR(CYAN, "Command read: CAP LS 302, automatic authentication process!");
		client->sendMessage("CAP * LS :\r\n");
		return;
	}
	processCommand(server, client, server->authCommands, 1006);
}

/*
According to the IRC protocol (both IRCv2 and IRCv3), commands like PASS, NICK, and USER 
are registration commands and are not allowed to be used after a client has successfully 
authenticated. Once a client has completed the registration process, these commands are 
ignored or result in an error message from the server.
*/
void CommandHandler::processCommand(Server* server, Client* client, CommandMap map, int errorNum)
{
	commandIt it = map.find(_command);
	if (it != map.end())
	{
		PRINT_COLOR(GREEN, "SUCCESS: Command ran: " << _command);
		(server->*it->second)(client, _params);
	}
	else
		throw (errorNum);
}
