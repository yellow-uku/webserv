#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &file) : file_path(file), servers(std::vector<Server>()) { }

void ConfigParser::parse()
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

	for (size_t i = 0; i < file.size(); ++i)
	{
		std::string temp;

		while (file[i] && std::isspace(file[i]))
			++i;

		if (file[i] == '#')
		{
			while (file[i] && file[i] != '\n')
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

	if (tokens.size() == 0)
		throw std::runtime_error("Error: Empty file");

	check_braces(tokens);

	parseLocations(tokens);
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
				throw std::runtime_error("Error: Wrong braces");
			br.pop();
		}
	}
	if (br.size() != 0)
		throw std::runtime_error("Error: Wrong braces");
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
		const std::string& upload_dir = location.getValueOf("upload_dir");

		if (location.getValueOf("root") == "")
		{
			location.setRoot(servers[server_index].getRoot());
		}
		if (upload_dir == "")
		{
			location.setUploadDir(servers[server_index].getRoot() + UPLOAD_DIRECTORY);
		}
		else if (upload_dir[0] != '/')
		{
			location.setUploadDir(location.getValueOf("root") + upload_dir);
		}
		if (location.getArrayOf("index").size() == 0)
		{
			location.pushIndexes("index.html");
		}
		if (location.getArrayOf("allow_methods").size() == 0)
		{
			location.pushMethods("GET");
			location.pushMethods("POST");
			location.pushMethods("DELETE");
		}
	}
}

void ConfigParser::setDefaults()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		std::map<int, std::string> err_pages = servers[i].getErrorPages();

		for (size_t j = 0; error_page_numbers[j]; ++j)
		{
			if (err_pages.find(error_page_numbers[j]) == err_pages.end())
			{
				servers[i].pushErrorPage(error_page_numbers[j], error_pages[j]);
			}
		}

		if (servers[i].getRoot() == "")
		{
			servers[i].setRoot(DEFAULT_ROOT);
		}
		if (servers[i].getServerNames().size() == 0)
		{
			servers[i].pushServerName("");
		}
		if (servers[i].getListens().size() == 0)
		{
			servers[i].pushListen(DEFAULT_HOST, DEFAULT_PORT);
		}
		if (servers[i].getMaxBodySize() == -1)
		{
			servers[i].setMaxBodySize(DEFAULT_MAX_BODY_SIZE);
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

	std::vector<std::string> current_location;

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (location_level == 0 && tokens[i] != "server")
			throw std::runtime_error("Error: Expected server block, found " + tokens[i]);

		if (tokens[i] == "server")
		{
			servers.push_back(Server());
			++location_level;

			++i;

			if (i == tokens.size() || tokens[i] != "{")
				throw std::runtime_error("Error: Invalid file");
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
			if (location_level == 1 && tokens[i] != "root")
				throw std::runtime_error("Error: " + tokens[i] + " is not allowed here");
			if (location_level == 1)
				root(tokens, server_index, i);
			else
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
				throw std::runtime_error("Error: Invalid location directive");
			storeLocation(tokens, current_location, location_level, server_index, i);
		}
		else if (tokens[i] != "}")
			throw std::runtime_error("Error: Invalid directive: " + tokens[i]);

		if (tokens[i] == "}" && location_level != 0)
		{
			--location_level;

			if (current_location.size())
				current_location.pop_back();
		}
		if (tokens[i] == "}" && location_level == 0)
		{
			++server_index;
		}
	}

	setDefaults();

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		const std::vector<std::string> names = it->getServerNames();

		const std::vector<listen_t> listens = it->getListens();

		for (std::vector<listen_t>::const_iterator lst = listens.begin(); lst != listens.end(); ++lst)
		{
			std::vector<Server>::iterator serv = std::find(it + 1, servers.end(), *lst);

			if (serv != servers.end())
			{
				for (size_t i = 0; i < names.size(); ++i)
				{
					if (std::find(it + 1, servers.end(), names[i]) != servers.end())
						throw std::runtime_error("Error: Duplicate server name: \"" + names[i] + "\"");
				}
			}
		}
	}
}

void ConfigParser::print()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		std::cout << "\t\tSERVER " << i + 1 << "\n\n";
		servers[i].print_everything();
		std::cout << "\n\n";
	}
}

ConfigParser::~ConfigParser() { }
