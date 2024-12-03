#include <Server.hpp>
#include <Client.hpp>
#include <Channel.hpp>
#include <sstream>
#include <algorithm>
#include <stdexcept>
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

void CommandHandler::processCommand(Server* server, Client* client, const std::string& raw_command)
{
	if (!server || !client || raw_command.empty())
		return;

	std::vector<std::string> parts = splitCommand(raw_command);
	if (parts.empty())
		return;

	std::string command = parts[0];
	//	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	//	Case sensitive?
	std::vector<std::string> params(parts.begin() + 1, parts.end());

	typedef void (*HandlerFunction)(Server*, Client*, const std::vector<std::string>&);
	std::vector<std::string> commands;
    commands.push_back("PASS");
    commands.push_back("NICK");
    commands.push_back("USER");
    commands.push_back("JOIN");
    commands.push_back("PRIVMSG");
    commands.push_back("PART");
    commands.push_back("QUIT");
    commands.push_back("KICK");
    commands.push_back("INVITE");
    commands.push_back("TOPIC");
    commands.push_back("MODE");

    std::vector<HandlerFunction> handlers;
    handlers.push_back(handlePass);
    handlers.push_back(handleNick);
    handlers.push_back(handleUser);
    handlers.push_back(handleJoin);
    handlers.push_back(handlePrivMsg);
    handlers.push_back(handlePart);
    handlers.push_back(handleQuit);
    handlers.push_back(handleKick);
    handlers.push_back(handleInvite);
    handlers.push_back(handleTopic);
    handlers.push_back(handleMode);

	//Handle unknown commands
    std::string error = "Unknown command: " + command;
}

void CommandHandler::handlePass(Server* server, Client* client, const std::vector<std::string>& params) 
{
	if (params.size() < 1)
	{
		client->sendMessage("Error: PASS requires a password");
		return;
	}
	else if (!client->authenticate(params[0], server->getPassword()))
		client->sendMessage("Error: Authentication failed");
}

void CommandHandler::handleNick(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1)
	{
		client->sendMessage("Error: NICK requires a nickname");
		return;
	}

	std::string nickname = params[0];
	// Check nickname validity (length, allowed characters)
	if (nickname.length() > 30 || nickname.empty()) {
		client->sendMessage("Error: Invalid nickname");
		return;
	}
	(void)server;
	/*
	if (server->isNicknameInUse(nickname))
	{
		client->sendMessage("Error: Nickname already in use");
		return;
	}
	*/


	client->setNickname(nickname);
}

void CommandHandler::handleUser(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 4)
	{
		client->sendMessage("Error: USER requires username, hostname, servername, and realname");
		//check if theyre not empty
		return;
	}
	(void)server;
	std::string username = params[0];
	std::string realname = params[3]; // realname is typically the last parameter

	client->setUsername(username, realname);
}

void CommandHandler::handleJoin(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 1) {
		client->sendMessage("Error: JOIN requires a channel name");
		return;
	}

	//std::string channelName = params[0];
	//Channel* channel = &Channel("pepe");
	//Channel* channel = server->getOrCreateChannel(channelName);

	try
	{
		//channel->addMember(client);
		//client->joinChannel(channel);
		(void)server;
	}
	catch (const std::exception& e)
	{
		client->sendMessage(std::string("Error: ") + e.what());
	}
}

void CommandHandler::handlePrivMsg(Server* server, Client* client, const std::vector<std::string>& params)
{
	if (params.size() < 2)
	{
		client->sendMessage("Error: PRIVMSG requires recipient and message");
		return;
	}

	std::string recipient = params[0];
	std::string message = params[1];

	Client* targetClient = server->findClient(recipient);
	Channel* targetChannel = server->findChannel(recipient);

	if (targetClient) {
		client->sendPrivateMessage(targetClient, message);
	} else if (targetChannel) {
		targetChannel->broadcastMessage(client, message);
	} else {
		client->sendMessage("Error: Recipient not found");
	}
}

void CommandHandler::handlePart(Server* server, Client* client, const std::vector<std::string>& params) {
	if (params.size() < 1) {
		client->sendMessage("Error: PART requires a channel name");
		return;
	}
	(void)client;
	(void)server;
	/*
	std::string channelName = params[0];
	Channel* channel = server->findChannel(channelName);

	if (channel) {
		channel->removeMember(client->getNickname());
		client->leaveChannel(channelName);
	}*/
}

// QUIT command: Disconnect from server
void CommandHandler::handleQuit(Server* server, Client* client, const std::vector<std::string>& params) {
	std::string quitMessage = params.empty() ? "Quit" : params[0];
	//server->removeClient(client, quitMessage);
	(void)client;
	(void)server;
}

// KICK command: Remove user from channel
void CommandHandler::handleKick(Server* server, Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		client->sendMessage("Error: KICK requires channel and nickname");
		return;
	}

	std::string channelName = params[0];
	std::string targetNickname = params[1];
	Channel* channel = server->findChannel(channelName);

	if (channel) {
		try {
			channel->kick(client, targetNickname);
		} catch (const std::exception& e) {
			client->sendMessage(std::string("Error: ") + e.what());
		}
	}
}

// INVITE command: Invite user to channel
void CommandHandler::handleInvite(Server* server, Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		client->sendMessage("Error: INVITE requires nickname and channel");
		return;
	}

	std::string targetNickname = params[0];
	std::string channelName = params[1];
	
	Client* targetClient = server->findClient(targetNickname);
	Channel* channel = server->findChannel(channelName);

	if (targetClient && channel) {
		try {
			channel->inviteUser(client, targetClient);
		} catch (const std::exception& e) {
			client->sendMessage(std::string("Error: ") + e.what());
		}
	}
}

// TOPIC command: Change or view channel topic
void CommandHandler::handleTopic(Server* server, Client* client, const std::vector<std::string>& params) {
	if (params.size() < 1) {
		client->sendMessage("Error: TOPIC requires channel name");
		return;
	}

	std::string channelName = params[0];
	Channel* channel = server->findChannel(channelName);

	if (channel) {
		if (params.size() > 1) {
			std::string newTopic = params[1];
			try {
				channel->setTopic(client, newTopic);
			} catch (const std::exception& e) {
				client->sendMessage(std::string("Error: ") + e.what());
			}
		} else {
			// If no new topic provided, show current topic
			client->sendMessage("Current topic for " + channelName + ": " + channel->getTopic());
		}
	}
}

// MODE command: Set channel or user modes
void CommandHandler::handleMode(Server* server, Client* client, const std::vector<std::string>& params) {
	if (params.size() < 2) {
		client->sendMessage("Error: MODE requires target and mode");
		return;
	}

	std::string target = params[0];
	std::string modeString = params[1];
	Channel* channel = server->findChannel(target);

	if (channel) {
		// Channel mode handling
		if (modeString == "+i") channel->setInviteOnly(true);
		else if (modeString == "-i") channel->setInviteOnly(false);
		else if (modeString == "+t") channel->setTopicRestricted(true);
		else if (modeString == "-t") channel->setTopicRestricted(false);
		// Add more mode handling as needed
	}
}