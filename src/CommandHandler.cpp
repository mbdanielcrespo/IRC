#include <CommandHandler.hpp>



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
	if (!server || !client || raw_command.empty())
		return;

	if (raw_command.compare(0, 10, "CAP LS 302") == 0)
	{
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(CYAN, "Command read: CAP LS 302, automatic authentication process!");
		processAutomaticAuth(server, client, raw_command);
		return;
	}

	std::vector<std::string> parts = splitCommand(raw_command);
	if (parts.empty())
		return;
	std::string command = parts[0];
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	std::vector<std::string> params(parts.begin() + 1, parts.end());
	
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
	std::string err_msg = "ERROR: command: " + raw_command + "Not found! Avaliable commands at the moment: PASS, NICK, USER!\n";
	client->sendMessage(err_msg);
	if (DEBUG == DEBUG_ON)
		PRINT_COLOR(RED, err_msg);
	return;
}
/*
According to the IRC protocol (both IRCv2 and IRCv3), commands like PASS, NICK, and USER 
are registration commands and are not allowed to be used after a client has successfully 
authenticated. Once a client has completed the registration process, these commands are 
ignored or result in an error message from the server.
*/

void CommandHandler::processCommand(Server* server, Client* client, const std::string& raw_command)
{
	if (!server || !client || raw_command.empty())
		return;

	std::vector<std::string> parts = splitCommand(raw_command);
	if (parts.empty())
		return;

	std::string command = parts[0];
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	std::vector<std::string> params(parts.begin() + 1, parts.end());

	typedef void (*HandlerFunction)(Server*, Client*, const std::vector<std::string>&);
	std::vector<std::string> commands;
	commands.push_back("JOIN");
	commands.push_back("PRIVMSG");
	commands.push_back("PART");
	commands.push_back("QUIT");
	commands.push_back("KICK");
	commands.push_back("INVITE");
	commands.push_back("TOPIC");
	commands.push_back("MODE");

	std::vector<HandlerFunction> handlers;
	handlers.push_back(handleJoin);
	handlers.push_back(handlePrivMsg);
	handlers.push_back(handlePart);
	handlers.push_back(handleQuit);
	handlers.push_back(handleKick);
	handlers.push_back(handleInvite);
	handlers.push_back(handleTopic);
	handlers.push_back(handleMode);

	for (size_t i = 0; i < commands.size(); i++)
	{
		if (command == commands[i])
		{
			PRINT_COLOR(GREEN, "SUCCESS: Command ran: " << command);
			handlers[i](server, client, params);
			return;	
		}
	}
	std::string err_msg = "ERROR: command: " + raw_command + "Not found! \r\n";
	client->sendMessage(err_msg);
	if (DEBUG == DEBUG_ON)
		PRINT_COLOR(RED, err_msg);
	return;
}

void CommandHandler::handlePass(Server* server, Client* client, const std::vector<std::string>& params) 
{
	if (params.size() < 1)
	{
		client->sendMessage("ERROR: PASS requires a password as parameter\n");
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(RED, "ERROR: PASS requires a password as parameter");
		return;
	}
	else if (!client->authenticate(params[0], server->getPassword()))
	{
		client->sendMessage("ERROR: Authentication failed, you must have a valid NICKname and USERname before setting password or pass is wrong\n");
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(RED, "ERROR: Authentication failed, you must have a valid NICKname and USERname before setting password or pass is wrong: " << server->getPassword() << "|" << params[0]);
	}
	else
	{
		client->sendMessage("Password authentication successfull! You can now send commands to this server!\n");
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(GREEN, "Password authentication successfull! You can now send commands to this server!");
	}
}


void CommandHandler::handleNick(Server* server, Client* client, const std::vector<std::string>& params)
{
	(void)server;
	if (params.size() < 1)
	{
		client->sendMessage(":server 431 * :No nickname given\r\n");
		return;
	}

	std::string nickname = params[0];
	if (nickname.length() > 60 || nickname.empty())
	{
		client->sendMessage("ERROR: Invalid nickname");
		return;
	}
	/*
	if (server->isNicknameInUse(nickname))
	{
		client->sendMessage("ERROR: Nickname already in use");
		return;
	}
	*/
	client->setNickname(nickname);
}


void CommandHandler::handleUser(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 4)
	{
		client->sendMessage("ERROR: USER requires username, hostname, servername, and realname\n");
		return;
	}
	(void)server;
	std::string username = params[0];
	//std::string realname = params[3]; // realname is typically the last parameter

	client->setUsername(username);
}


void CommandHandler::handleJoin(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
	{
		client->sendMessage("ERROR: JOIN requires a channel name \r\n");
		return;
	}

	std::string channelName = params[0];
	Channel *channel = server->findChannel(channelName, client);
	if (!channel)
	{
		if (DEBUG == DEBUG_ON)
			PRINT_ERROR(RED, "ERROR: CHANNEL was never created");
		return;
	}
	try
	{
		client->joinChannel(channel);
		std::string joinMessage = ":" + client->getId() + " JOIN :" + channelName + "\r\n";
		client->sendMessage(joinMessage);
	}
	catch (const std::exception& e)
	{
		PRINT_COLOR(RED, "ERROR: " << e.what());
		client->sendMessage(std::string("ERROR: ") + e.what());
	}
}

void CommandHandler::handlePrivMsg(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		client->sendMessage("ERROR: PRIVMSG requires recipient and message! \r\n");
		return;
	}

	std::string recipient = params[0];
	std::string message = params[1];

	Client* targetClient = server->findClient(recipient);
	Channel* targetChannel = server->findChannel(recipient, NULL);

	if (targetClient == client)
		client->sendMessage("ERROR: Recipient cannot be yourself! \r\n");


	if (targetClient)
		client->sendPrivateMessage(targetClient, message);
	else if (targetChannel)
		targetChannel->broadcastMessage(client, message);
	else
		client->sendMessage("ERROR: Recipient not found! \r\n");
}

void CommandHandler::handlePart(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
	{
		client->sendMessage("ERROR: PART requires a channel name \r\n");
		return;
	}
	
	std::string channelName = params[0];
	Channel* channel = server->findChannel(channelName, NULL);

	if (!channel->isMember(client->getNickname()))
	{
		client->sendMessage("ERROR: You're not part of " + channel->getName() + " channel! \r\n");
		if (DEBUG == DEBUG_ON)
			PRINT_ERROR(RED, client->getNickname() + "isnt part of " + channel->getName() + " channel!");
	}

	if (channel)
	{
		client->leaveChannel(channelName);
		channel->removeMember(client->getNickname());
		if (DEBUG == DEBUG_ON)
			PRINT_COLOR(BLUE, client->getNickname() + " left the channel " + channelName);
	}
}

void CommandHandler::handleQuit(Server* server, Client* client, const std::vector<std::string>& params)
{
    if (!server || !client)
		return;

    std::string quitMessage = "Client " + client->getNickname() + " has quit \r\n";
    if (!params.empty())
        quitMessage = params[0];

    PRINT_COLOR(RED, "Client " << client->getNickname() << " is quitting: " << quitMessage);

    std::map<std::string, Channel*> joinedChannels = client->getJoinedChannels();
    for (std::map<std::string, Channel*>::iterator it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
    {
        Channel* channel = it->second;
        if (channel)
		{
            channel->broadcastMessage(client, "QUIT :" + quitMessage);
            channel->removeMember(client->getNickname());
        }
    }

	if (DEBUG == DEBUG_ON)
    	PRINT_COLOR(GREEN, "Client " << client->getNickname() << " successfully removed from server.");
    server->removeClient(client->getSocketFd());
}

void CommandHandler::handleKick(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		client->sendMessage("ERROR: KICK requires channel and nickname!");
		return;
	}

	std::string channelName = params[0];
	std::string targetNickname = params[1];
	Channel* channel = server->findChannel(channelName, NULL);

	if (channel)
	{
		try
		{
			channel->kick(client, targetNickname);
		}
		catch (const std::exception& e)
		{
			client->sendMessage(std::string("ERROR: ") + e.what());
		}
	}
	else
		PRINT_ERROR(RED, "ERROR: KICK a channel!");
}


void CommandHandler::handleInvite(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		client->sendMessage("ERROR: INVITE requires nickname and channel! \r\n");
		return;
	}

	std::string targetNickname = params[0];
	std::string channelName = params[1];
	
	Client* targetClient = server->findClient(targetNickname);
	Channel* channel = server->findChannel(channelName, NULL);

	if (client == targetClient)
	{
		client->sendMessage("ERROR: You cannot invite yourself! \r\n");
		return;
	}

	if (targetClient && channel)
	{
		try
		{
			channel->inviteUser(client, targetClient);
			if (DEBUG == DEBUG_ON)
				PRINT_COLOR(CYAN, "Invited " + targetClient->getNickname() + " successfully!");
		}
		catch (const std::exception& e)
		{
			client->sendMessage(std::string("ERROR: ") + e.what());
		}
	}
	else
		PRINT_ERROR(RED, "INVITE Requires a targetClient && channel!");
}

// TOPIC command: Change or view channel topic
void CommandHandler::handleTopic(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
	{
		client->sendMessage("ERROR: TOPIC requires channel name! \r\n");
		return;
	}

	std::string channelName = params[0];
	Channel* channel = server->findChannel(channelName, NULL);

	if (channel)
	{
		if (params.size() > 1)
		{
			std::string newTopic = params[1];
			try
			{
				channel->setTopic(client, newTopic);
			}
			catch (const std::exception& e)
			{
				client->sendMessage(std::string("ERROR: ") + e.what());
			}
		}
		else
			client->sendMessage("Current topic for " + channelName + ": " + channel->getTopic() + "\r\n");
	}
}

// MODE command: Set channel or user modes
void CommandHandler::handleMode(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		client->sendMessage("ERROR: MODE requires target and mode");
		return;
	}

	std::string target = params[0];
	std::string modeString = params[1];
	Channel* channel = server->findChannel(target, NULL);

	if (channel)
	{
		if (modeString == "+i") channel->setInviteOnly(true);
		else if (modeString == "-i") channel->setInviteOnly(false);
		else if (modeString == "+t") channel->setTopicRestricted(true);
		else if (modeString == "-t") channel->setTopicRestricted(false);
	}
	else
		PRINT_ERROR(RED, "ERROR: MODE requires target and mode!");

}