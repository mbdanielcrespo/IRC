/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 11:04:46 by marvin            #+#    #+#             */
/*   Updated: 2024/11/07 11:04:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <Client.hpp>
#include <main.h>


class Channel {
	private:
		std::string name;
		std::string topic;
		bool inviteOnly;
		int userLimit;
		std::string password;
		std::vector<Client*> members;
		std::vector<Client*> operators;

	public:
		Channel(const std::string &name);

		void addMember(Client *client);
		void removeMember(Client *client);
		bool isMember(Client *client) const;

		void addOperator(Client *client);
		bool isOperator(Client *client) const;

		void kick(Client *operatorClient, Client *targetClient);
		void invite(Client *operatorClient, Client *targetClient);
		void setTopic(Client *operatorClient, const std::string &newTopic);

		void setInviteOnly(bool value);
		void setUserLimit(int limit);
		void setPassword(const std::string &channelPassword);

		std::string getName() const;
		std::string getTopic() const;
		bool getInviteOnly() const;
		int getUserLimit() const;
		std::string getPassword() const;
};


