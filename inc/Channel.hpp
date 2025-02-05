#pragma once
#include <string>
#include <map>
#include <vector>

class Client;

class Channel
{
	private:
		std::string		_name;
		std::string		_topic;
		bool			_inviteOnly;
		bool			_topicRestricted;
		bool			_hasKey;
		std::string		_key;
		size_t			_userLimit;
		Client*			_topicSetter;

		std::map<std::string, Client*>	_members;
		std::map<std::string, Client*>	_operators;
		std::vector<std::string>		_invitedUsers;

	public:
		Channel(const std::string& name);
		~Channel();

		void		addMember(Client* client);
		void		removeMember(const std::string& nickname);
		bool		isMember(const std::string& nickname) const;

		void		addOperator(Client* client);
		void		removeOperator(const std::string& nickname);
		bool		isOperator(const std::string& nickname) const;

		void		setInviteOnly(bool mode);
		void		setTopicRestricted(bool mode);
		void		setKey(const std::string& key);
		void		setUserLimit(size_t limit);
		void 		setTopicSetter(Client *client);

		void		broadcastMessage(Client* sender, const std::string& message);
		void		inviteUser(Client* inviter, Client* invited);

		void		kick(Client* kicker, const std::string& nickname);
		void		setTopic(Client* setter, const std::string& topic);

		std::string	getName() const;
		std::string	getTopic() const;
		size_t		getMemberCount() const;
		bool		getHasKey() const;
		int 		getMemberLimit() const;
		Client		*getTopicSetter() const;

		void 		checkOperator(Client* client);
		void 		checkClient(Client* client);
		void 		checkUserLimit(void);
		void 		checkKey(const std::vector<std::string>& params);
		void 		checkInviteOnly(void);

		std::string listMembers() const;
		std::string listOperators() const;
};