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
	"error_pages/408.html",
	"error_pages/411.html",
	"error_pages/413.html",
	"error_pages/500.html",
	NULL,
};

const int ConfigParser::error_page_numbers[] = {
	400,
	403,
	404,
	405,
	408,
	411,
	413,
	500,
	0,
};
