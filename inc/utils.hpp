#pragma once

#include <iostream>
#include <string>
#include <vector>

template <typename Container>
void printContainer(const Container& cont)
{
	typename Container::const_iterator it = cont.begin();
	typename Container::const_iterator end = cont.end();

	while (it != end)
	{
		std::cout << *it;
		++it;
		if (it != end)
			std::cout << " ";
	}
	std::cout << std::endl;
}

std::string findAndReplace(std::string str, const std::string& old_string, const std::string& new_string);
std::vector<std::string> splitStr(const std::string& str, char delimiter);
std::string joinStr(const std::vector<std::string>& tokens, const std::string& delimiter);