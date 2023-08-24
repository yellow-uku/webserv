#include "ConfigParser.hpp"

char const * const ConfigParser::single_value_directives_location[] = {
	"autoindex",
	"root",
	"return",
	"cgi",
	"upload_dir",
	NULL,
};

char const * const ConfigParser::array_value_directives_location[] = {
	"index",
	"allow_methods",
	NULL,
};

char const * const ConfigParser::allowed_methods[] = {
	"GET",
	"POST",
	"DELETE",
	NULL,
};

bool ConfigParser::contains(char const * const allowed[], const std::string &token)
{
	for (size_t i = 0; allowed[i]; ++i)
	{
		if (token == allowed[i])
			return true;
	}

	return false;
}
