# include "../headers/response_headers.hpp"

response_headers::response_headers()
{
	http_versus	= "HTTP/1.1 ";
	http_status	= "200 OK";
	connection	= "Close";
	content_type	= "text/html";
	backslash_r_n	= "\r\n";
}

response_headers::~response_headers()
{
}
void response_headers::build_headers()
{
	headers = http_versus + http_status + backslash_r_n 
		+ "Connection: " + connection + backslash_r_n
		+ "Content-Type: " + content_type + backslash_r_n
		+ backslash_r_n;
	//std::cout << "*************\n" << headers << std::endl;
}
