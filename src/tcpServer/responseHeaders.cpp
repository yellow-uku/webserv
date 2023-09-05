#include "TCPserver.hpp"

Response_headers::Response_headers()
{
	http_versus	= "HTTP/1.1 ";
	http_status	= "200 OK";
	connection	= "Close";
	content_type	= "text/html";
	backslash_r_n	= "\r\n";
}

Response_headers::~Response_headers()
{}

void Response_headers::build_headers()
{
	headers = http_versus + http_status + backslash_r_n 
		+ "Connection: " + connection + backslash_r_n
		+ "Content-Type: " + content_type + backslash_r_n
		+ backslash_r_n;
}
