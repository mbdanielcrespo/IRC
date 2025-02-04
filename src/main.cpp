/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 10:52:44 by marvin            #+#    #+#             */
/*   Updated: 2024/11/07 10:52:44 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <iostream>
#include <string>

std::string handleError(int errorCode)
{
	std::string errorMessage;
	switch (errorCode)
	{
		case 431:
			errorMessage = "ERR_NONICKNAMEGIVEN: No nickname given";
			break;
		case 432:
			errorMessage = "ERR_ERRONEUSNICKNAME: Erroneous nickname";
			break;
		case 433:
			errorMessage = "ERR_NICKNAMEINUSE: Nickname is already in use";
			break;
		case 436:
			errorMessage = "ERR_NICKCOLLISION: Nickname collision";
			break;
		case 451:
			errorMessage = "ERR_NOTREGISTERED: You have not registered";
			break;
		case 461:
			errorMessage = "ERR_NEEDMOREPARAMS: Not enough parameters";
			break;
		case 464:
			errorMessage = "ERR_PASSWDMISMATCH: Password incorrect";
			break;
		case 421:
			errorMessage = "ERR_UNKNOWNCOMMAND: Unknown command";
			break;
		case 402:
			errorMessage = "ERR_NOSUCHSERVER: Server not found";
			break;
		case 401:
			errorMessage = "ERR_NOSUCHNICK: No such nick/channel";
			break;

		// JOIN command errors
		case 403:
			errorMessage = "ERR_NOSUCHCHANNEL: No such channel";
			break;
		case 405:
			errorMessage = "ERR_TOOMANYCHANNELS: You have joined too many channels";
			break;
		case 471:
			errorMessage = "ERR_CHANNELISFULL: Cannot join channel (+l)";
			break;
		case 473:
			errorMessage = "ERR_INVITEONLYCHAN: Cannot join channel (+i)";
			break;
		case 474:
			errorMessage = "ERR_BANNEDFROMCHAN: Cannot join channel (+b)";
			break;
		case 475:
			errorMessage = "ERR_BADCHANNELKEY: Cannot join channel (+k)";
			break;

		// PRIVMSG command errors
		case 411:
			errorMessage = "ERR_NORECIPIENT: No recipient given";
			break;
		case 412:
			errorMessage = "ERR_NOTEXTTOSEND: No text to send";
			break;
		case 404:
			errorMessage = "ERR_CANNOTSENDTOCHAN: Cannot send to channel";
			break;
		case 407:
			errorMessage = "ERR_TOOMANYTARGETS: Too many recipients";
			break;

		// PART command errors
		case 442:
			errorMessage = "ERR_NOTONCHANNEL: You're not on that channel";
			break;

		case 1001:
			errorMessage = "ERR_EMPTYCMD: Command is empty";
			break;
		case 1002:
			errorMessage = "ERR_CLIENTNOTVALID: Client not valid";
			break;
		case 1003:
			errorMessage = "ERR_SERVERNOTVALID: Server not valid";
			break;
		case 1004:
			errorMessage = "Client has quit";
			break;
		case 1005:
			errorMessage = "ERR_NOTYOURSELF: Recipient cannot be yourself!";
			break;
		default:
			errorMessage = "Unknown error code!";
			PRINT_ERROR(RED, errorCode);
			break;
	}
	return errorMessage + "\n";
}


int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}

	int port = std::atoi(argv[1]);
	std::string password = argv[2];

	Server server(port, password);
	server.run();

	return EXIT_SUCCESS;
}
