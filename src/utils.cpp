#include <utils.hpp>

std::string findAndReplace(std::string str, const std::string& old_str, const std::string& new_str)
{
    size_t pos = 0;
    while ((pos = str.find(old_str, pos)) != std::string::npos)
	{
        str.replace(pos, old_str.length(), new_str);
        pos += new_str.length(); // Move past the replacement to avoid infinite loop
    }
    return str;
}

std::vector<std::string> splitStr(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t pos = 0;

    while ((pos = str.find(delimiter, start)) != std::string::npos)
    {
        tokens.push_back(str.substr(start, pos - start));
        start = pos + 1;
    }
    if (start < str.length())
        tokens.push_back(str.substr(start));
    return tokens;
}

std::string joinStr(const std::vector<std::string>& tokens, const std::string& delimiter)
{
    std::string result;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        result += tokens[i];
        if (i < tokens.size() - 1)
            result += delimiter;
    }
    return result;
}
