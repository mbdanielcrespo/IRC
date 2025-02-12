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

		void		broadcastMessage(Client* sender, const std::string& message);
		void		broadcastLogMessage(Client* sender, const std::string& message);

		void		addInvitedUser(const std::string& nickname);
		bool		isInvited(const std::string& nickname) const;
		void		inviteUser(Client* inviter, Client* invited);
		void		kick(Client* kicker, const std::string& nickname);

		void		setTopic(Client* setter, const std::string& topic);
		void		setInviteOnly(bool mode);
		void		setTopicRestricted(bool mode);
		void		setKey(const std::string& key, bool flag);
		void		setUserLimit(const std::string& limit, bool flag);
		void 		setTopicSetter(Client *client);
		void		setHasKey(bool flag);
		std::string	getName( void ) const;
		std::string	getTopic( void ) const;
		size_t		getMemberCount( void ) const;
		bool		getHasKey( void ) const;
		int 		getMemberLimit( void ) const;
		Client		*getTopicSetter( void ) const;

		void 		checkOperator(Client* client);
		void 		checkClient(Client* client);
		void 		checkUserLimit(void);
		void 		checkKey(const std::vector<std::string>& params);
		void 		checkInviteOnly( Client *client );
		void		checkTopicRestricted( Client *client );


		std::string listMembers() const;
		std::string listOperators() const;


};