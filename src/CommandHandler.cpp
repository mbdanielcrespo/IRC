#include <CommandHandler.hpp>
#include <Channel.hpp>


void CommandHandler::checkChannel(Channel *channel)
{
	if (channel == NULL)
		throw (403);
}

std::vector<std::string> CommandHandler::splitCommand(const std::string& raw_command)
{
	std::vector<std::string> parts;
	std::istringstream iss(raw_command);
	std::string part;

	while (iss >> part)
		parts.push_back(part);
	return parts;
}

void processAutomaticAuth(Server* server, Client* client, const std::string& raw_command)
{
	std::vector<std::string> cmds = splitStr(raw_command, '\n');
	//token = findAndReplace(token, " ", "_");
	std::vector<std::string>::iterator it = cmds.begin();
	*it++; // CAP LS 302
	while (it != cmds.end())
	{
		CommandHandler::processAuth(server, client, *it);
		it++;
	}
}

void CommandHandler::processAuth(Server* server, Client* client, const std::string& raw_command)
{
	try
	{
		if (!server)
			throw(402);
		if (!client)
			PRINT_ERROR(RED, handleError(401));
		if (raw_command.empty())
			throw(1001);

		if (raw_command.compare(0, 10, "CAP LS 302") == 0)
		{
			PRINT_COLOR(CYAN, "Command read: CAP LS 302, automatic authentication process!");
			client->sendMessage("CAP * LS :\r\n");
			processAutomaticAuth(server, client, raw_command);
			return;
		}

		/*  O SPLITSTR NAO FUNCIONA CORRETAMENTE
		for (size_t i = 0; raw_command[i] != ' ';i++)
			std::cout << "("<< (int)raw_command[i] << ") "<< '\'' << raw_command[i] << "\' ";
		std::cout << std::endl;

		std::vector<std::string> arguments = splitStr(raw_command, ' ');

		for (size_t i = 0;i < arguments[1].size();i++)
			std::cout << "("<< (int)arguments[1][i] << ") "<< '\'' << arguments[1][i] << "\' ";
		std::cout << std::endl;
		*/
		std::vector<std::string> arguments = splitCommand(raw_command);
		if (arguments.empty())
			throw (1001);

		std::string command = arguments[0];
		std::transform(command.begin(), command.end(), command.begin(), ::toupper);
		std::vector<std::string> params(arguments.begin() + 1, arguments.end());

		std::vector<std::string> commands;
		commands.push_back("PASS");
		commands.push_back("NICK");
		commands.push_back("USER");

		std::vector<HandlerFunction> handlers;
		handlers.push_back(handlePass);
		handlers.push_back(handleNick);
		handlers.push_back(handleUser);

		for (size_t i = 0; i < commands.size(); i++)
		{
			if (command == commands[i])
			{
				PRINT_COLOR(GREEN, "SUCCESS: Command ran: " << command);
				handlers[i](server, client, params);
				return;	
			}
		}
		throw(1006); // UNKOWN COMMAND
	}
	catch(int err)
	{
		client->sendMessage(handleError(err));
	}
}

/*
According to the IRC protocol (both IRCv2 and IRCv3), commands like PASS, NICK, and USER 
are registration commands and are not allowed to be used after a client has successfully 
authenticated. Once a client has completed the registration process, these commands are 
ignored or result in an error message from the server.
*/
void CommandHandler::processCommand(Server* server, Client* client, const std::string& raw_command)
{
	try
	{
		if (!server)
			throw(402);
		if (!client)
			PRINT_ERROR(RED, handleError(401));
		if (raw_command.empty())
			throw(1001);

		std::vector<std::string> arguments = splitCommand(raw_command);	
		if (arguments.empty())
			throw (1001);

		std::string command = arguments[0];
		std::transform(command.begin(), command.end(), command.begin(), ::toupper);
		std::vector<std::string> params(arguments.begin() + 1, arguments.end());

		std::vector<std::string> commands;
		commands.push_back("JOIN");
		commands.push_back("PRIVMSG");
		commands.push_back("PART");
		commands.push_back("QUIT");
		commands.push_back("KICK");
		commands.push_back("INVITE");
		commands.push_back("TOPIC");
		commands.push_back("MODE");
		commands.push_back("NICK");
		commands.push_back("USER");

		std::vector<HandlerFunction> handlers;
		handlers.push_back(handleJoin);
		handlers.push_back(handlePrivMsg);
		handlers.push_back(handlePart);
		handlers.push_back(handleQuit);
		handlers.push_back(handleKick);
		handlers.push_back(handleInvite);
		handlers.push_back(handleTopic);
		handlers.push_back(handleMode);
		handlers.push_back(handleNick);
		handlers.push_back(handleUser);

		for (size_t i = 0; i < commands.size(); i++)
		{
			if (command == commands[i])
			{
				PRINT_COLOR(GREEN, "SUCCESS: Command ran: " << command);
				handlers[i](server, client, params);
				return;	
			}
		}
		throw(421);
	}
	catch(int err)
	{
		client->sendMessage(handleError(err));
	}
}

/*
void CommandHandler::handleMode(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
		throw(461);

	std::string target = params[0];
	std::string modeString = params[1];
	Channel* channel = server->findChannel(target);

	checkChannel(channel);
	if (!channel->isOperator(client->getNickname()))
		throw(482);

	if (modeString == "+k" && params.size() < 3) throw(476);
	else if (modeString == "+k" && params.size() == 3) channel->setKey(params[2]);
	else if (modeString == "-k") channel->setKey("");
	else if (modeString == "+i") channel->setInviteOnly(true);
	else if (modeString == "-i") channel->setInviteOnly(false);
	else if (modeString == "+t") channel->setTopicRestricted(true);
	else if (modeString == "-t") channel->setTopicRestricted(false);
	else if (modeString == "+l" && params.size() < 3) throw(461);
	else if (modeString == "+l" && params.size() == 3)
	{
		std::istringstream iss(params[2]);
		size_t val;
		if (!(iss >> val))
			throw(476);
		channel->setUserLimit(val);
	}
	else if (modeString == "-l") channel->setUserLimit(-1);
	else if (modeString == "+o" && params.size() < 3) throw(476);
	else if (modeString == "+o" && params.size() == 3)
	{
		Client *new_op = server->findClient(params[2]);
		channel->checkClient(new_op);
		if (!channel->isMember(params[2]))
			throw(441);
		if (channel->isOperator(new_op->getNickname()))
			throw(1006);	
		channel->addOperator(new_op);
	}
	else if (modeString == "-o" && params.size() < 3) throw(476);
	else if (modeString == "-o" && params.size() == 3)
	{
		Client *new_op = server->findClient(params[2]);
		channel->checkClient(new_op);
		if (!channel->isMember(params[2]))
			throw(441);
		channel->removeOperator(params[2]);
	}
	else throw(472);
}
*/