#include "ConfigParser.hpp"

void ConfigParser::getHostValues(const std::string& line, std::string& key, std::string &value) const
{
	size_t i = 0;

	while (line[i] && std::isspace(line[i]))
		++i;

	if (line[i] == '#')
		return ;

	while (line[i] && !std::isspace(line[i]))
		key.push_back(line[i++]);

	while (line[i] && std::isspace(line[i]))
		++i;

	while (line[i] && !std::isspace(line[i]))
		value.push_back(line[i++]);
}

void ConfigParser::getHosts()
{
	std::ifstream file("/etc/hosts");

	if (file.fail())
		return;

	std::string line;
	while (std::getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(line);

		getHostValues(line, key, value);
		hosts.insert(std::make_pair(value, key));
	}
	hosts["localhost"] = "::1";
}
