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

char const * const ConfigParser::error_pages[] = {
	"error_pages/400.html",
	"error_pages/403.html",
	"error_pages/404.html",
	"error_pages/405.html",
	"error_pages/413.html",
	NULL,
};

const int ConfigParser::error_page_numbers[] = {
	400,
	403,
	404,
	405,
	413,
	0,
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
