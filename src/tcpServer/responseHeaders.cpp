#include "TCPserver.hpp"

ResponseHeaders::ResponseHeaders()
{
	http_versus	= "HTTP/1.1 ";
	http_status	= "200 OK";
	connection	= "Close";
	content_type	= "text/html";
	backslash_r_n	= "\r\n";
}

ResponseHeaders::~ResponseHeaders() { }

void ResponseHeaders::build_headers()
{
	headers = http_versus + http_status + backslash_r_n 
		+ "Connection: " + connection + backslash_r_n
		+ "Content-Type: " + content_type + backslash_r_n
		+ backslash_r_n;
}
