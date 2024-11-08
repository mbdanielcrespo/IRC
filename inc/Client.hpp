/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 10:54:39 by marvin            #+#    #+#             */
/*   Updated: 2024/11/07 10:54:39 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <main.h>

class Client 
{
	private:
		int _socket_fd;
		std::string _nickname;
		std::string _username;
		bool _authenticated;

	public:
		Client(int socket_fd);
		~Client();

		// Setters
		void setNickname(const std::string &nickname);
		void setUsername(const std::string &username);
		void authenticate();

		// Getters
		int getSocket() const;
		std::string getNickname() const;
		std::string getUsername() const;
		bool isAuthenticated() const;
};

