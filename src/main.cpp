#include "Server.hpp"

std::string handleError(int errorCode)
{
	std::string errorMessage;
	switch (errorCode)
	{
		case 401:
			errorMessage = "ERR_NOSUCHNICK: No such nick/channel";						break;
		case 402:
			errorMessage = "ERR_NOSUCHSERVER: Server not found";						break;
		case 403:
			errorMessage = "ERR_NOSUCHCHANNEL: No such channel";						break;
		case 404:
			errorMessage = "ERR_CANNOTSENDTOCHAN: Cannot send to channel";				break;
		case 405:
			errorMessage = "ERR_TOOMANYCHANNELS: You have joined too many channels";	break;
		case 407:
			errorMessage = "ERR_TOOMANYTARGETS: Too many recipients";					break;
		case 411:
			errorMessage = "ERR_NORECIPIENT: No recipient given";						break;
		case 412:
			errorMessage = "ERR_NOTEXTTOSEND: No text to send";							break;
		case 421:
			errorMessage = "ERR_UNKNOWNCOMMAND: Unknown command";						break;
		case 431:
			errorMessage = "ERR_NONICKNAMEGIVEN: No nickname given";					break;
		case 432:
			errorMessage = "ERR_ERRONEUSNICKNAME: Erroneous nickname";					break;
		case 433:
			errorMessage = "ERR_NICKNAMEINUSE: Nickname is already in use";				break;
		case 436:
			errorMessage = "ERR_NICKCOLLISION: Nickname collision";						break;
		case 441:
			errorMessage = "ERR_USERNOTINCHANNEL: They aren’t on that channel";			break;
		case 442:
			errorMessage = "ERR_NOTONCHANNEL: You're not on that channel";				break;
		case 451:
			errorMessage = "ERR_NOTREGISTERED: You have not registered";				break;
		case 461:
			errorMessage = "ERR_NEEDMOREPARAMS: Not enough parameters";					break;
		case 464:
			errorMessage = "ERR_PASSWDMISMATCH: Password incorrect";					break;
		case 467:
			errorMessage = "ERR_KEYSET: Channel key already set";						break;
		case 471:
			errorMessage = "ERR_CHANNELISFULL: Cannot join channel (+l)";				break;
		case 472:
			errorMessage = "ERR_UNKNOWNMODE: Unknown mode";								break;
		case 473:
			errorMessage = "ERR_INVITEONLYCHAN: Cannot join channel (+i)";				break;
		case 474:
			errorMessage = "ERR_BANNEDFROMCHAN: Cannot join channel (+b)";				break;
		case 475:
			errorMessage = "ERR_BADCHANNELKEY: Cannot join channel (+k)";				break;
		case 479:
			errorMessage = "ERR_BADCHANNAME: :Invalid channel name";					break;
		case 482:
			errorMessage = "ERR_CHANOPRIVSNEEDED: You're not a channel operator";		break;
		case 696:
			errorMessage = "ERR_PARAMKEYMODE: You must specify a parameter for the key mode.";			break;
		case 1001:
			errorMessage = "ERR_EMPTYCMD: Command is empty";							break;
		case 1002:
			errorMessage = "ERR_CLIENTNOTVALID: Client not valid";						break;
		case 1003:
			errorMessage = "ERR_SERVERNOTVALID: Server not valid";						break;
		case 1004:
			errorMessage = "Client has quit";											break;
		case 1005:
			errorMessage = "ERR_NOTYOURSELF: Recipient cannot be yourself!";			break;
		case 1006:
			errorMessage = "ERR_NOTLOGED: Please register with PASS before using any commands.";		break;
		case 1007://todo
			errorMessage = "ERR_ALREADYOP: User is an operator already!";				break;
		case 1008://todo
			errorMessage = "ERR_NONICKSET: No nick set";								break;
		case 1009://todo
			errorMessage = "ERR_NOUSERSET: No user set";								break;
		default:
			errorMessage = "Unknown error code!";
			PRINT_ERROR(RED, errorCode);												break;
	}
	return (errorMessage + "\n");
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		PRINT_ERROR(RED, "Usage: " + (std::string)(argv[0]) + " <port> <password>\n");
		return EXIT_FAILURE;
	}

	int port = std::atoi(argv[1]);

	try{
		if (port > 65535)
			throw ((std::string)(argv[0]) + ": Port number too large " + (std::string)argv[1]);
		else if (port < 1)
			throw ((std::string)(argv[0]) + ": Port number too small " + (std::string)argv[1]);
		else if (port < 1024)
			throw ((std::string)("Error: Invalid port! must be between 1024 and 65535"));
	}
	catch (std::string &error){
		PRINT_ERROR(RED, error);
		return EXIT_FAILURE;
	}
	
	try{
		std::string password = argv[2];
		Server server(port, password);
		server.run();
	}
	catch (std::runtime_error &e){
		PRINT_ERROR(RED, e.what()); 
	}


	return EXIT_SUCCESS;
}
