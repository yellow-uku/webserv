#include "ConfigParser.hpp"

void ConfigParser::setter(Location& location, const std::string& name, const std::string& val)
{
	if (name == "autoindex")
	{
		if (val != "on" && val != "off")
			throw std::runtime_error("invalid value of autoindex directive");
		location.setAutoindex(val);
	}
	else if (name == "root")
		location.setRoot(val);
	else if (name == "return")
		location.setHttpRedir(val);
	else if (name == "cgi")
		location.setCgi(val);
	else if (name == "upload_dir")
		location.setUploadDir(val);
	else if (name == "index")
		location.pushIndexes(val);
	else if (name == "allow_methods")
	{
		if (!contains(allowed_methods, val))
			throw std::runtime_error("invalid value of allow_methods directive");
		location.pushMethods(val);
	}
}

void ConfigParser::setProperties(Location& current_location, const std::vector<std::string>& tokens, size_t& i)
{
	std::string directive_name = tokens[i];

	size_t count = ++i;

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		setter(current_location, directive_name, tokens[count]);
		++count;
	}

	if (count == i || count - 1 != i || tokens[count] != ";")
		throw std::runtime_error("invalid " + directive_name + " directive");

	i = count;
}

void ConfigParser::setVectors(Location& current_location, const std::vector<std::string>& tokens, size_t& i)
{
	std::string directive_name = tokens[i];

	size_t count = ++i;

	while (tokens[count] !=  ";" && tokens[count] != "{" && tokens[count] != "}")
	{
		setter(current_location, directive_name, tokens[count]);
		++count;
	}

	if (count == i || tokens[count] != ";")
		throw std::runtime_error("invalid " + directive_name + " directive");

	i = count;
}

void ConfigParser::storeLocation(const std::vector<std::string>& tokens, std::vector<std::string>& current_location, size_t& location_level, const size_t& server_index, size_t& i)
{
	if (location_level == 1)
	{
		++i;
		current_location.push_back(tokens[i]);
		
		Location temp;

		LocationMap::iterator it = servers[server_index].locations.find(tokens[i]);

		if (it != servers[server_index].locations.end() && it->second == temp)
			throw std::runtime_error("Duplicate location ay txa e o");

		servers[server_index].locations[tokens[i]] = temp;

		++i;
		++location_level;
	}
	else
	{
		++i;
		if (IS_OUTSIDE_LOCATION(tokens[i], current_location.back()))
			throw std::runtime_error(tokens[i] + " is outside location " + current_location.back());

		LocationMap::iterator it = servers[server_index].locations.find(tokens[i]);

		Location temp(location_level, current_location.back());

		if (it != servers[server_index].locations.end() && it->second == temp)
			throw std::runtime_error("Duplicate location ay txa e o");

		servers[server_index].locations.insert(std::make_pair(tokens[i], temp));

		current_location.push_back(tokens[i]);
		++i;
		++location_level;
	}
}
