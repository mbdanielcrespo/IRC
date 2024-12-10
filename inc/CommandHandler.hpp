#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <Server.hpp>
#include <Client.hpp>
#include <Channel.hpp>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <utils.hpp>


class Server;
class Client;
class Channel;

typedef void (*HandlerFunction)(Server*, Client*, const std::vector<std::string>&);

class CommandHandler
{
	public:
		static void	processCommand(Server* server, Client* client, const std::string& raw_command);
		static void	processAuth(Server* server, Client* client, const std::string& raw_command);

	private:
		static std::vector<std::string> splitCommand(const std::string& raw_command);

		static void	handlePass(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleNick(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleUser(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handlePrivMsg(Server* server, Client* client, const std::vector<std::string>& params);

		static void	handleJoin(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handlePart(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleQuit(Server* server, Client* client, const std::vector<std::string>& params);

		static void	handleKick(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleInvite(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleTopic(Server* server, Client* client, const std::vector<std::string>& params);
		static void	handleMode(Server* server, Client* client, const std::vector<std::string>& params);
};