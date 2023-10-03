#include "ConfigParser.hpp"

void ConfigParser::validationOfListen(std::string token, std::string& host, std::string& port) const
{
	std::string	_host;
	std::string	_port;

	if (token.find(':') != std::string::npos)
	{
		if (token.find(':') != token.rfind(':'))
			throw std::runtime_error("Invalid count of colons in listen directive");

		std::stringstream ss(token);

		std::getline(ss, _host, ':');
		std::getline(ss, _port);

		if (!_host[0])
			throw std::runtime_error("No host in listen directive");

		int count;

		if ((count = isValidIP(_host)) != -1)
			host = storeIP(_host, count);
		else if (hosts.find(_host) == hosts.end())
			throw std::runtime_error("Host not found in listen directive");
		else
			host = hosts.find(_host)->second;

		if (!isValidPort(_port))
			throw std::runtime_error("Invalid port in listen directive");
		port = _port;
	}
	else
	{
		if (!isValidPort(token))
			throw std::runtime_error("Invalid port in listen directive");
		port = token;
	}
}

int ConfigParser::isValidIP(const std::string& host) const
{
	int	count = 0;

	if (!std::isdigit(host[0]) || !std::isdigit(host[host.length() - 1]))
		return -1;

	for (size_t i = 0; i < host.length() - 1; i++)
	{
		if ((!std::isdigit(host[i]) && host[i] != '.') || (host[i] == '.' && host[i + 1] == '.'))
			return -1;
		if (host[i] == '.')
			count++;
	}

	if (count > 3)
		return -1;
	return count;
}

std::string ConfigParser::storeIP(const std::string& host, int count) const
{
	std::string ret;
	std::vector<std::string> ip_vector = splitIP(host, count, '.');

	ip_vector.insert(ip_vector.begin() + count, 3 - count, "0");

	for (size_t i = 0; i < ip_vector.size(); ++i)
	{
		ret += ip_vector[i];
		ret += i == ip_vector.size() - 1 ? "" : ".";
	}

	return ret;
}

bool ConfigParser::isValidPort(const std::string& _port) const
{
	if (std::find_if(_port.begin(), _port.end(), NonDigit()) != _port.end())
		return false;

	int p = my_stoi(_port);

	return !(p <= 0 || p > 65535);
}
