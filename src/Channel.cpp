#include <Channel.hpp>
#include <Client.hpp>
#include <algorithm>
#include <stdexcept>

Channel::Channel(const std::string& name) :
	_name(name),
	_topic(""),
	_inviteOnly(false),
	_topicRestricted(false),
	_hasKey(false),
	_key(""),
	_userLimit(0)
{
	if (name.empty())
		throw std::invalid_argument("Channel name cannot be empty");
	
	if (name[0] != '#')
		throw std::invalid_argument("Channel name must start with '#'");
}

Channel::~Channel()
{
	_members.clear();
	_operators.clear();
	_invitedUsers.clear();
}

void Channel::checkClient(Client* client)
{
	if (client == NULL)
		throw std::invalid_argument("Invalid client!");
}

void Channel::checkOperator(Client* client)
{
	if (!isOperator(client->getNickname()))
		throw std::runtime_error("Client is not a channel operator");
}

void Channel::addMember(Client* client)
{
	checkClient(client);
	
	if (_userLimit > 0 && _members.size() >= _userLimit)
		throw std::runtime_error("Channel user limit reached");
	
	if (_inviteOnly && 
		std::find(_invitedUsers.begin(), _invitedUsers.end(), client->getNickname()) == _invitedUsers.end())
		throw std::runtime_error("Channel is invite-only");
	
	// In a real implementation, key verification would happen before this method
	if (_hasKey && !_key.empty())
		throw std::runtime_error("Channel requires a key to join");
	
	_members[client->getNickname()] = client;
	broadcastMessage(client, "has joined the channel");
}

void Channel::removeMember(const std::string& nickname)
{
	std::map<std::string, Client*>::iterator it = _members.find(nickname);
	
	if (it != _members.end())
	{
		broadcastMessage(it->second, "has left the channel");
		_members.erase(it);
		removeOperator(nickname);
	}
}

void Channel::addOperator(Client* client)
{
	checkClient(client);
	if (!isMember(client->getNickname()))
		throw std::runtime_error("Only channel members can be operators");
	
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
	checkClient(sender); // return or throw?
	
	std::string fullMessage = ":" + sender->getPrefix() + " " + message;
	
	for (std::map<std::string, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (it->second != sender)
			it->second->sendMessage(fullMessage);
	}
}

void Channel::inviteUser(Client* inviter, Client* invited)
{
	checkClient(invited);
	checkClient(invited);
	checkOperator(inviter);
	
	_invitedUsers.push_back(invited->getNickname());
	invited->sendMessage(":" + inviter->getPrefix() + " INVITE " + 
						 invited->getNickname() + " :" + _name);
}

void Channel::kick(Client* kicker, const std::string& nickname)
{
	checkClient(kicker);
	checkOperator(kicker);

	std::map<std::string, Client*>::iterator it = _members.find(nickname);
	if (it != _members.end())
	{
		broadcastMessage(kicker, "KICK " + nickname + " from the channel");
		removeMember(nickname);
	}
	//else
	// member does not exist
}

void Channel::setTopic(Client* setter, const std::string& topic)
{
	checkClient(setter);
	
	if (_topicRestricted && !isOperator(setter->getNickname()))
		throw std::runtime_error("Only operators can change topic in restricted mode");
	
	_topic = topic;
	broadcastMessage(setter, "TOPIC " + _name + " :" + topic);
}

void Channel::setInviteOnly(bool mode)
{
	_inviteOnly = mode;
}

void Channel::setTopicRestricted(bool mode)
{
	_topicRestricted = mode;
}

void Channel::setKey(const std::string& key)
{
	_key = key;
	_hasKey = !key.empty();
}

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
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
	return _topic;
}

size_t Channel::getMemberCount() const
{
	return _members.size();
}