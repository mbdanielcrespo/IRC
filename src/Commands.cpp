#include <CommandHandler.hpp>
#include <Channel.hpp>


// DONE
void CommandHandler::handlePass(Server* server, Client* client, const std::vector<std::string>& params) 
{
	if (params.size() < 1)
		throw(461);
	else if (!client->authenticate(params[0], server->getPassword()))
		throw(451);
	else
		client->sendMessage("Password authentication successfull!\nYou can now send commands to this server!\n");
}

/* TO FIX:

<< NICK "	ou 		<< NICK #ASD
>> @time=2025-02-08T18:01:54.520Z :irc-5e48.darkscience.net 432 user " :Erroneous Nickname

*/
void CommandHandler::handleNick(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (!server)
		throw(402);
	if (!client)
		PRINT_ERROR(RED, handleError(401));
	if (params.size() < 1)
		throw(431);

	std::string nickname = params[0];
	if (nickname.length() > 60 || nickname.empty())
		throw(432);
	if (server->isNicknameInUse(nickname))
		throw(433);
	client->setNickname(nickname);
		PRINT_COLOR(GREEN,
		"NICK: " + client->getNickname() + "\n" +
		"USER: " + client->getUsername() + "\n"
	);
}


void CommandHandler::handleUser(Server* server, Client* client, const std::vector<std::string>& params)
{
	(void)server;
	if (params.size() < 4)
	{
		PRINT_COLOR(RED, "Requires: username hostname servername realname");
		throw(461);
	}
	std::string username = params[0];
	client->setUsername(username);
	PRINT_COLOR(GREEN,
		"NICK: " + client->getNickname() + "\n" +
		"USER: " + client->getUsername() + "\n"
	);
}

// TEST
void CommandHandler::handleJoin(Server* server, Client* client, const std::vector<std::string>& params)
{
	std::stringstream ss(params[0]);
	std::string channelName;

	if (params.size() < 1)
		throw(461);

	while (std::getline(ss, channelName, ',')) {
		server->checkChannelName(channelName, client);
		Channel *channel = server->findChannel(channelName, client);
		if (channel == NULL)
			server->createChannel(channelName, client);
		channel->checkInviteOnly();
		channel->checkUserLimit();
		if (channel->getHasKey())
			channel->checkKey(params);

		// o cliente e adicionado, a msg e mandada a todo o channel -> o cliente vai receber a sua provpria entrada
		client->joinChannel(channel);
		client->sendMessage(":" + client->getId() + " JOIN :" + channelName + "\r\n");
	}
}

// DONE
void CommandHandler::handlePart(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
		throw(461);
	
	std::string channelName = params[0];
	Channel* channel = server->findChannel(channelName, NULL);

	checkChannel(channel);

	if (!channel->isMember(client->getNickname()))
		throw(442);

	client->leaveChannel(channelName);
	channel->removeMember(client->getNickname());
}

// DONE
void CommandHandler::handleKick(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
		throw(461);

	std::string channelName = params[0];
	std::string targetNickname = params[1];
	Channel* channel = server->findChannel(channelName, NULL);

	if (!channel)
		throw(401);

	channel->kick(client, targetNickname);
}


void CommandHandler::handleQuit(Server* server, Client* client, const std::vector<std::string>& params)
{
    if (!server)
		throw(402);
	if (!client)
		throw(401);

	std::string quitMessage;
	if (!params.empty())
	    quitMessage = "Client " + client->getNickname() + " has quit \r\n";
	else
		quitMessage = "Client " + client->getNickname() + ": " + params[0] + "\r\n";

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

    server->removeClient(client->getSocketFd());
}

void CommandHandler::handleInvite(Server* server, Client* client, const std::vector<std::string>& params)
{
	// TODO: CHECK USER LIMIT
	if (params.size() < 2)
		throw(461);

	std::string targetNickname = params[0];
	std::string channelName = params[1];
	
	Client* targetClient = server->findClient(targetNickname);
	Channel* channel = server->findChannel(channelName, NULL);

	channel->checkClient(targetClient);
	checkChannel(channel);

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

void CommandHandler::handleTopic(Server* server, Client* client, const std::vector<std::string>& params)
{
	Channel		*channel;
	std::string	topic;
	std::string	new_topic;

	if (params.size() == 0)
		throw(401);
	channel = server->findChannel(params[0], client);
	checkChannel(channel);
	// TODO: SEND CORRECT MESSAGE TO CLIENT
	if (params.size() == 1 && channel->getTopicSetter() == NULL)
		client->sendMessage(":" + client->getId() + " 331 " + client->getNickname() + " :" + channel->getName() + " :" + channel->getTopic() + "\r\n");
	else if (params.size() == 1)
	{
		client->sendMessage(":" + client->getId() + " 332 " + client->getNickname() + " :" + channel->getName() + " :" + channel->getTopic() + "\r\n");
		client->sendMessage(":" + client->getId() + " 333 " + client->getNickname() + " :" + channel->getName() + " " + (channel->getTopicSetter())->getId() + " :1738766800" + "\r\n");
	}
	if (params.size() >= 2)
	{
		new_topic = params[1];
		client->sendMessage(":" + client->getId() +  " TOPIC " + channel->getName() + " " + new_topic + "\r\n");
		channel->setTopic(client, new_topic);
	}
}

// TODO: TEST
void CommandHandler::handlePrivMsg(Server* server, Client* client, const std::vector<std::string>& params)
{
	std::string recipient;
	std::string message;

	if (params.size() < 1)
		throw(411);
	if (params.size() < 2)	
		throw(412);
		
	recipient = params[0];
	message = params[1];
	
	Client* targetClient = server->findClient(recipient);
	Channel* targetChannel = server->findChannel(recipient, NULL);

	if (targetClient == client)
		throw(1005);

	if (targetClient)
		client->sendPrivateMessage(targetClient, message);
	else if (targetChannel)
		targetChannel->broadcastMessage(client, message);
	else
		throw(411);
}

// TODO: TEST
void CommandHandler::handleMode(Server* server, Client* client, const std::vector<std::string>& params)
{
	bool flag;

	if (params.size() < 2)
		throw(461);

	std::string target = params[0];
	std::string modeString = params[1];
	Channel* channel = server->findChannel(target, NULL);

	checkChannel(channel);
	if (!channel->isOperator(client->getNickname()))
		throw(482);

	if (modeString == "+k" && params.size() < 3)
		throw(476);
	else if (modeString == "+k" && params.size() == 3)
		channel->setKey(params[2]);
	/*switch(modeString)
	{
		case "-k" : channel->setKey("");
		default: break;
	}*/
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

//TODO: WHO (handle)
//TODO: SEND SERVER CORRECT MESSAGE FOR NICK FOR BEING ABLE TO JOIN CHANNELS AFTERWARDS
//TODO: TEST MODES AND ALL OTHER COMMANDS WITH MMODES	