#include <Channel.hpp>
#include <Client.hpp>
#include <algorithm>
#include <stdexcept>
#include <sstream>

Channel::Channel(const std::string& name) :
	_name(name),
	_topic(""),
	_inviteOnly(false),
	_topicRestricted(false),
	_hasKey(false),
	_key(""),
	_userLimit(-1),
	_topicSetter(NULL),

	_members(),    
	_operators(),
	_invitedUsers()
{}

Channel::~Channel()
{
	_members.clear();
	_operators.clear();
	_invitedUsers.clear();
}

void Channel::checkClient(Client* client)
{
	if (client == NULL)
		throw (401);
}

void Channel::checkOperator(Client* client)
{
	if (!isOperator(client->getNickname()))
		throw (482);
}

void Channel::checkUserLimit(void)
{
	if (this->getMemberCount() >= this->_userLimit)
		throw(471);
}

void Channel::checkKey(const std::vector<std::string>& params)
{
	if (_hasKey)
	{
		if (params.size() < 2 ||  _key != params[1])
		{
			PRINT_COLOR(RED, _key + " | " + params[1]);
			throw(475);
		}
	}
}

void Channel::checkInviteOnly(Client *client)
{
	if (_inviteOnly)
	{
		if (!this->isInvited(client->getNickname()))
			throw(473);
	}
}

void Channel::checkTopicRestricted(Client *client)
{
	if (_topicRestricted)
	{
		if (!this->isOperator(client->getNickname()))
			throw(482);
	}
}

void Channel::addMember(Client* client)
{
	checkClient(client);

	client->sendMessage(listMembers());
	client->sendMessage(listOperators());
	client->sendMessage(":ircserv 366 " + client->getNickname() + " " + this->getName() + " :End of names list\r\n");

	broadcastMessage(client, "has joined the channel \r\n");
	_members[client->getNickname()] = client;
}

void Channel::removeMember(const std::string& nickname)
{
	std::map<std::string, Client*>::iterator it = _members.find(nickname);
	
	if (it != _members.end())
	{
		std::string PartMessage = ":" + it->second->getId() + " PART " + this->getName() + " :Leaving\r\n";
		it->second->sendMessage(PartMessage);
		broadcastMessage(it->second, "PART " + this->getName() + " : Leaving\r\n");
		_members.erase(it);
		removeOperator(nickname);
	}
}

void Channel::addOperator(Client* client)
{
	checkClient(client);
	if (!isMember(client->getNickname()))
		throw std::runtime_error("Only channel members can be operators");		// TROCAR PELO ERROR CODE CORRETO
	_operators[client->getNickname()] = client;
}

void Channel::removeOperator(const std::string& nickname)
{
	std::map<std::string, Client*>::iterator it = _operators.find(nickname);
	
	if (it != _operators.end())
		_operators.erase(it);
}

void Channel::broadcastMessage(Client* sender, const std::string& message)
{
	checkClient(sender);

	std::string fullMessage = ":" + sender->getId() + " PRIVMSG " + this->getName() + " " + message + "\r\n";
	PRINT_COLOR(YELLOW, "Broadcasting to " << _members.size() << " members.");

	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		Client* recipient = it->second;

		if (recipient)
		{
			if (recipient != sender)
				recipient->sendMessage(fullMessage);
		}
	}
}

void Channel::broadcastLogMessage(Client* sender, const std::string& message)
{
	checkClient(sender);

	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		Client* recipient = it->second;

		if (recipient)
		{
			if (recipient != sender)
				recipient->sendMessage(message);
		}
	}
}

void Channel::addInvitedUser(const std::string& nickname)
{
    if (std::find(_invitedUsers.begin(), _invitedUsers.end(), nickname) == _invitedUsers.end())
    {
        _invitedUsers.push_back(nickname);
        PRINT_COLOR(GREEN, "User " + nickname + " has been added to the invite list for " + _name);
    }
}

bool Channel::isInvited(const std::string& nickname) const
{
	if (std::find(_invitedUsers.begin(), _invitedUsers.end(), nickname) != _invitedUsers.end())
    	return true;
	return false;
}

void Channel::inviteUser(Client* inviter, Client* invited)
{
	checkClient(inviter);
	checkClient(invited);
	checkOperator(inviter);
	
	this->addInvitedUser(invited->getNickname());
	std::string joinMessage = ":" + inviter->getId() + " INVITE " + invited->getNickname() + " :" + _name + "\r\n";
	invited->sendMessage(joinMessage);
}

void Channel::kick(Client* kicker, const std::string& nickname)
{
	checkClient(kicker);
	checkOperator(kicker);

	std::map<std::string, Client*>::iterator it = _members.find(nickname);
	if (it != _members.end())
	{
		broadcastMessage(kicker, "KICK " + nickname + " from the channel \r\n");
		removeMember(nickname);
	}
}

void Channel::setTopic(Client* setter, const std::string& topic)
{
	checkClient(setter);
	
	if (_topicRestricted && !isOperator(setter->getNickname()))
		throw (482);
	_topicSetter = setter;
	_topic = topic;
	broadcastMessage(setter, "TOPIC set to: " + topic);
}

void Channel::setInviteOnly(bool mode)
{
	_inviteOnly = mode;
	if (mode)
		PRINT_COLOR(BLUE, "Channel INVITE only set to: true");
	else
		PRINT_COLOR(BLUE, "Channel INVITE only set to: false");
}

void Channel::setTopicRestricted(bool mode)
{
	_topicRestricted = mode;
	if (mode)
		PRINT_COLOR(BLUE, "Channel TOPIC resctriction set to: true");
	else
		PRINT_COLOR(BLUE, "Channel TOPIC resctriction set to: false");
}

void Channel::setTopicSetter(Client *client)
{
	_topicSetter = client;
}

void Channel::setHasKey(bool flag)
{
	_hasKey = flag;
}

void Channel::setKey(const std::string& key, bool flag)
{
	if (flag == true)
	{
		_key = key;
		_hasKey = true;
		PRINT_COLOR(BLUE, "Channel KEY set to: " + key);
		return ;
	}

	_key = "";
	_hasKey = false;
	PRINT_COLOR(BLUE, "Channel KEY Unset");
}

void Channel::setUserLimit(const std::string& limit, bool flag)
{
	if (flag == true)  //protect max? // proteger para limit = ""
	{
		_userLimit = std::atoi(limit.c_str());
		PRINT_COLOR(BLUE, "Channel USERLIMIT set to: " + limit);
		return ;
	}
	_userLimit = -1;
	PRINT_COLOR(BLUE, "Channel USERLIMIT unset");
}

bool Channel::isMember(const std::string& nickname) const
{
	return _members.find(nickname) != _members.end();
}

bool Channel::isOperator(const std::string& nickname) const
{
	return _operators.find(nickname) != _operators.end();
}

std::string Channel::getName() const
{
	return _name;
}

std::string Channel::getTopic() const
{
	if (_topic.empty())
		return "Channel has no topic assigned!";
	return _topic;
}

size_t Channel::getMemberCount() const
{
	return _members.size();
}

bool Channel::getHasKey() const
{
	return _hasKey;
}

bool Channel::getInviteOnly() const
{
	return _inviteOnly;
}

int Channel::getMemberLimit() const
{
	return _userLimit;
}

bool Channel::getHasLimit() const
{
	if (_userLimit != (size_t)-1)
		return true;
	return false;
}

bool Channel::getTopicRestricted() const
{
	return _topicRestricted;
}


Client*	Channel::getTopicSetter() const
{
	return _topicSetter;
}

void	Channel::changeOperatorStatus(Client *new_op, std::string clientName, bool flag)
{
	this->checkClient(new_op);
	if (!this->isMember(clientName))
		throw(441);

	if (flag == true)
		this->addOperator(new_op);
	else
		this->removeOperator(clientName);
}

std::string Channel::listMembers() const
{
	std::ostringstream oss;
	oss << getMemberCount();
	std::string memberList = "Members in channel " + _name + " (" + oss.str() + "): \n";

	for (std::map<std::string, Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		memberList += "-> " + it->first + "\n";
	}	

	memberList += "\r\n";
	return memberList;
}

std::string Channel::listOperators() const
{
	std::string operatorList = "Operators in channel " + _name + ": \n";

	for (std::map<std::string, Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		operatorList += "-> " + it->first + "\n";
	}

	operatorList += "\r\n";
	return operatorList;
}
