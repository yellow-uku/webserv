#include "ConfigParser.hpp"

void ConfigParser::root(const std::vector<std::string>& tokens, size_t& server_index, size_t& i)
{
	size_t count = ++i;

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		servers[server_index].setRoot(tokens[count]);

		++count;
	}

	if (count == i || count - 1 != i || tokens[count] != ";")
		throw std::runtime_error("invalid root directive");

	i = count;
}

void ConfigParser::serverName(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i)
{
	if (location_level != 1)
		throw std::runtime_error(tokens[i] + " is not allowed here");

	size_t count = ++i;

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		servers[server_index].pushServerName(tokens[count]);
		++count;
	}

	if (count == i || tokens[count] != ";")
		throw std::runtime_error("invalid server_name directive");

	i = count;
}

void ConfigParser::listen(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i)
{
	if (location_level != 1)
		throw std::runtime_error(tokens[i] + " is not allowed here");

	size_t count = ++i;


	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		std::string	host("");
		std::string	port("8080");

		validationOfListen(tokens[count], host, port);

		servers[server_index].pushListen(host, port);

		++count;
	}

	if (count == i || count - 1 != i || tokens[count] != ";")
		throw std::runtime_error("invalid listen directive");

	i = count;
}

void ConfigParser::errorPage(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i)
{
	if (location_level != 1)
		throw std::runtime_error(tokens[i] + " is not allowed here");

	size_t		count = ++i;
	std::string	error_page;

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		++count;
	}

	error_page = tokens[count - 1];
	if (containsSpecialChar(error_page))
		throw std::runtime_error("invalid path in error_page directive");

	if (count == i || count == i + 1 || tokens[count] != ";")
		throw std::runtime_error("invalid error_page directive");

	count = i;

	while (tokens[count] != error_page)
	{
		int error_code = my_stoi(tokens[count]);

		if (error_code < 300 || error_code >= 600)
			throw std::runtime_error("invalid error_code in error_page directive");

		servers[server_index].pushErrorPage(error_code, error_page);
		++count;
	}

	i = count + 1;
}

void ConfigParser::maxBodySize(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i)
{
	if (location_level != 1)
		throw std::runtime_error(tokens[i] + " is not allowed here");

	size_t count = ++i;

	if (servers[server_index].getMaxBodySize() != -1)
		throw std::runtime_error("duplicate client_max_body_size directive");

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		servers[server_index].setMaxBodySize(getActualBodySize(tokens[count]));
		++count;
	}

	if (count == i || count - 1 != i || tokens[count] != ";")
		throw std::runtime_error("invalid client_max_body_size directive");

	i = count;
}

bool ConfigParser::containsSpecialChar(const std::string& token)
{
	const std::string specChars("*?:;\"\'<>|&%#$@+-=");

	for (std::string::const_iterator it = token.begin(); it != token.end(); ++it)
	{
		std::string::const_iterator found = std::find(specChars.begin(), specChars.end(), *it);
		if (found != specChars.end())
			return true;
	}
	return false;
}

// k: Kilobytes	-> 10^3
// m: Megabytes	-> 10^6
// g: Gigabytes	-> 10^9
size_t ConfigParser::getActualBodySize(const std::string& token)
{
	if (!std::isdigit(token[token.size() - 1]))
	{
		char		unit = std::tolower(token[token.size() - 1]);
		std::string	val = token.substr(0, token.length() - 1);
		int			to_int = my_stoi(val);

		if (to_int < 0)
			throw std::runtime_error("Error: invalid max_body_size value");

		size_t power = 1;

		if (unit == 'k')
			power = std::pow(1024, 1);
		else if (unit == 'm')
			power = std::pow(1024, 2);
		else if (unit == 'g')
			power = std::pow(1024, 3);
		else
			throw std::runtime_error("Error: invalid max_body_size value");

		if ((unit == 'g' && to_int > MAX_BODY_SIZE_G)
			|| (unit == 'm' && to_int > MAX_BODY_SIZE_M)
			|| (unit == 'k' && to_int > MAX_BODY_SIZE_K))
			throw std::runtime_error("Error: max_body_size value too large");

		return static_cast<size_t>(to_int) * power;
	}

	int to_int = my_stoi(token);

	if (to_int < 0)
		throw std::runtime_error("Error: invalid max_body_size value");

	return static_cast<size_t>(to_int);
}
