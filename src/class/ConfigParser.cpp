#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &file_path) : servers(std::vector<Server>()) { parse(file_path); }

void ConfigParser::parse(const std::string &file_path)
{
	std::ifstream conf(file_path.c_str());
	std::stringstream sstream;

	if (conf.fail())
		throw std::runtime_error(strerror(errno));

	getHosts();

	sstream << conf.rdbuf();

	std::string file(sstream.str());
	
	generateTokens(file);
}

void ConfigParser::generateTokens(const std::string &file)
{
	std::vector<std::string> tokens;

	std::cout << "\n";
	for (size_t i = 0; i < file.size(); ++i)
	{
		std::string temp;

		while (file[i] && std::isspace(file[i]))
			++i;

		if (file[i] == '#')
		{
			while (file[i] != '\n')
				++i;
			continue ;
		}

		while (file[i] && !std::isspace(file[i]) && file[i] != ';' && file[i] != '{' && file[i] != '}')
		{
			temp.push_back(file[i]);
			++i;
		}

		if (temp.size())
			tokens.push_back(temp);

		if (file[i] == ';' || file[i] == '{' || file[i] == '}')
			tokens.push_back(std::string(1, file[i]));
	}
	check_braces(tokens);

	parseLocations(tokens);
	std::cout << "\n";
}

void ConfigParser::check_braces(const std::vector<std::string> tokens)
{
	std::stack<int> br;

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i] == "{")
			br.push(0);
		else if (tokens[i] == "}")
		{
			if (br.size() == 0)
				throw std::runtime_error("wrong braces");
			br.pop();
		}
	}
	if (br.size() != 0)
		throw std::runtime_error("wrong braces");
}

void ConfigParser::init_locations(Location& location, const size_t& server_index)
{
	if (location.getParent() != "")
	{
		init_locations((servers[server_index].locations.find(location.getParent()))->second, server_index);
		location = (servers[server_index].locations.find(location.getParent()))->second;
	}
	else
	{
		if (location.getArrayOf("index").size() == 0)
		{
			location.pushIndexes("index.html");
		}
		if (location.getArrayOf("allow_methods").size() == 0)
		{
			location.pushMethods("GET");
			location.pushMethods("POST");
		}
	}
}

void ConfigParser::setDefaults()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		if (servers[i].getErrorPages().size() == 0)
		{
			servers[i].pushErrorPage(403, DEFAULT_403_PAGE);
			servers[i].pushErrorPage(404, DEFAULT_404_PAGE);
		}
		if (servers[i].getServerNames().size() == 0)
		{
			servers[i].pushServerName("");
		}
		if (servers[i].getListens().size() == 0)
		{
			servers[i].pushListen(DEFAULT_HOST, DEFAULT_PORT);
		}
		for (LocationMap::reverse_iterator it = servers[i].locations.rbegin(); it != servers[i].locations.rend(); ++it)
		{
			init_locations(it->second, i);
		}
	}
}

void ConfigParser::parseLocations(std::vector<std::string> &tokens)
{
	size_t server_index = 0;
	size_t location_level = 0;
	size_t location_index = 0;

	std::vector<std::string> current_location;

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (location_level == 0 && tokens[i] != "server")
			throw std::runtime_error("expected server block, found " + tokens[i]);

		if (tokens[i] == "server")
		{
			servers.push_back(Server());
			++location_level;

			++i;

			if (i == tokens.size() || tokens[i] != "{")
				throw std::runtime_error("invalid file");
		}
		else if (tokens[i] == "server_name")
			serverName(tokens, server_index, location_level, i);
		else if (tokens[i] == "listen")
			listen(tokens, server_index, location_level, i);
		else if (tokens[i] == "client_max_body_size")
			maxBodySize(tokens, server_index, location_level, i);
		else if (tokens[i] == "error_page")
			errorPage(tokens, server_index, location_level, i);
		else if (contains(single_value_directives_location, tokens[i]))
		{
			if (location_level <= 1)
				throw std::runtime_error(tokens[i] + " is not allowed here");
 			setProperties(servers[server_index].locations[current_location.back()], tokens, i);
		}
		else if (contains(array_value_directives_location, tokens[i]))
		{
			if (location_level <= 1)
				throw std::runtime_error(tokens[i] + " is not allowed here");
			setVectors(servers[server_index].locations[current_location.back()], tokens, i);
		}
		else if (tokens[i] == "location")
		{
			if (tokens[i + 1] == "{" || tokens[i + 2] != "{")
				throw std::runtime_error("invalid location directive");
			storeLocation(tokens, current_location, location_level, server_index, i);
		}
		else if (tokens[i] != "}")
			throw std::runtime_error("invalid directive: " + tokens[i]);

		if (tokens[i] == "}" && location_level != 0)
		{
			--location_level;

			if (current_location.size())
				current_location.pop_back();
			if (location_level == 1)
				++location_index;
		}
		if (tokens[i] == "}" && location_level == 0)
		{
			++server_index;
			location_index = 0;
		}
	}

	setDefaults();
}

ConfigParser::~ConfigParser() { }
