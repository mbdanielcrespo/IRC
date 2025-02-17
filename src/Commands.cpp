#include "Server.hpp"

void Server::handlePass(Client* client, const std::vector<std::string>& params) 
{
	if (params.size() < 1)
		throw(461);
	if (client->authenticate(params[0], _pass))
		client->sendMessage("Password authentication successfull!\nYou can now send commands to this server!\n");
	else
		throw(451);
}

void Server::handleNick(Client* client, const std::vector<std::string>& params)
{
	if (!client)
		PRINT_ERROR(RED, handleError(401));
	if (params.size() < 1)
		throw(431);

	std::string nickname = params[0];
	if (nickname.length() > 60 || nickname.empty())
		throw(432);
	if (this->isNicknameInUse(nickname))
		throw(433);
	client->setNickname(nickname);
		PRINT_COLOR(GREEN,
		"NICK: " + client->getNickname() + "\n" +
		"USER: " + client->getUsername() + "\n"
	);
}

void Server::handleUser(Client* client, const std::vector<std::string>& params)
{
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

void Server::handleJoin(Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
		throw(461);
	
	std::string channelName;
	std::stringstream ss(params[0]);

	while (std::getline(ss, channelName, ',')) {
		this->checkChannelName(channelName, client);
		Channel *channel = this->findChannel(channelName);
		if (channel == NULL)
			channel = this->createChannel(channelName, client);
		channel->checkInviteOnly(client);
		channel->checkUserLimit();
		channel->checkKey(params);

		client->joinChannel(channel);
		client->sendMessage(":" + client->getId() + " JOIN :" + channelName + "\r\n");
	}
}

void Server::handlePart(Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
		throw(461);

	std::string channelName;
	std::stringstream ss(params[0]);

	while (std::getline(ss, channelName, ',')) {
		Channel* channel = this->findChannel(channelName);
		checkChannel(channel);

		if (!channel->isMember(client->getNickname()))
			throw(442);

		client->leaveChannel(channelName);
		channel->removeMember(client->getNickname());

		//TODO: if the last client leaves the channel is erased?
	}
}

void Server::handleKick(Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
		throw(461);

	std::string channelName = params[0];
	std::string targetNickname = params[1];
	Channel* channel = this->findChannel(channelName);
	checkChannel(channel);

	channel->kick(client, targetNickname);
}


void Server::handleQuit(Client* client, const std::vector<std::string>& params)
{
	std::string quitMessage = "Client " + client->getNickname();
	if (params.empty())
		quitMessage += " has quit \r\n";
	else
		quitMessage += ": " + params[0] + "\r\n";

	std::map<std::string, Channel*> joinedChannels = client->getJoinedChannels();
	for (std::map<std::string, Channel*>::iterator it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
	{
		Channel* channel = it->second;
		channel->broadcastMessage(client, "QUIT :" + quitMessage);
		if (channel->isMember(client->getNickname()))
			channel->removeMember(client->getNickname());
	}
	
	this->removeClient(client->getSocketFd());
}

void Server::handleInvite(Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
		throw(461);

	std::string targetNickname = params[0];
	std::string channelName = params[1];
	
	Client* targetClient = this->findClient(targetNickname);
	Channel* channel = this->findChannel(channelName);

	channel->checkClient(targetClient);
	checkChannel(channel);

	if (client == targetClient)
	{
		client->sendMessage("ERROR: You cannot invite yourself! \r\n");
		return;
	}

	if (targetClient && channel)
	{
		channel->inviteUser(client, targetClient);
		PRINT_COLOR(CYAN, "Invited " + targetClient->getNickname() + " successfully!");
	}
	else
		PRINT_ERROR(RED, "INVITE Requires a targetClient && channel!");
}

void Server::handleTopic(Client* client, const std::vector<std::string>& params)
{
	Channel		*channel;
	std::string	topic;
	std::string	new_topic;

	if (params.size() == 0)
		throw(401);
	channel = this->findChannel(params[0]);
	checkChannel(channel);
	channel->checkTopicRestricted(client);
	
	if (params.size() == 1 && channel->getTopicSetter() == NULL)
		client->sendMessage(":" + client->getId() + " 331 " + client->getNickname() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");
	else if (params.size() == 1)
	{
		client->sendMessage(":" + client->getId() + " 332 " + client->getNickname() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");		// TODO: mudar numero para timestamp correta (em numero)
		client->sendMessage(":" + client->getId() + " 333 " + client->getNickname() + " " + channel->getName() + " " + (channel->getTopicSetter())->getId() + " :1738766800" + "\r\n");
	}
	if (params.size() >= 2)
	{
		new_topic = params[1];
		client->sendMessage(":" + client->getId() +  " TOPIC " + channel->getName() + " " + new_topic + "\r\n");
		channel->setTopic(client, new_topic);
	}
}

void Server::handlePrivMsg(Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
		throw(411);
	if (params.size() < 2)	
		throw(412);
		
	std::string recipient = params[0];
	std::string message = params[1];
	
	Client* targetClient = this->findClient(recipient);
	Channel* targetChannel = this->findChannel(recipient);

	if (targetClient == client)
		throw(1005);

	if (targetClient)
		client->sendPrivateMessage(targetClient, message);
	else if (targetChannel)
		targetChannel->broadcastMessage(client, message);
	else
		throw(411);
}

static bool getSymbol(char c)
{
	if (c != '+' && c != '-')
		throw (472);
	if (c == '+')
		return (true);
	return (false);
}

void Server::handleMode(Client* client, const std::vector<std::string>& params)
{
	bool flag = false;

	if (params.size() == 0)
		throw (461);

	Channel* channel = this->findChannel(params[0]);
	checkChannel(channel);

	if (params.size() == 1)
		client->sendChannelModes(channel);

	std::string modeString = params[1];

	if (!channel->isOperator(client->getNickname()))
		throw(482);
	if (modeString.size() != 2)
		throw(472);

	flag = getSymbol(modeString[0]); //TODO: checks if the flag is valid 
	switch(modeString[1])
	{
		case 'i':	channel->setInviteOnly(flag);						break;
		case 't':	channel->setTopicRestricted(flag);					break;
		default: 
			if (params.size() < 3)
				throw (696);

			switch(modeString[1])
			{
				case 'k':	channel->setKey(params[2], flag);										break;
				case 'l':	channel->setUserLimit(params[2], flag);									break;
				case 'o':	channel->changeOperatorStatus(this->findClient(params[2]), params[2], flag);		break;
				default:	throw(472);																break;
			}
			break;
	}
}
