#ifndef UTIL_HPP
# define UTIL_HPP

# include <vector>
# include <sstream>
# include <iostream>
# include <algorithm>

struct listen_t;

struct socket_t
{
	int fd;
	std::string host;
	std::string port;

	bool operator==(const int& socket) const ;
	bool operator==(const socket_t& lhs) const ;
	bool operator==(const listen_t& lhs) const ;
	bool operator==(const std::string& lhs) const ;

	socket_t(): fd(-1) { }
	socket_t(const std::string& hst, const std::string& prt): fd(-1), host(hst), port(prt) { }
	socket_t(const int& socket, const std::string& hst, const std::string& prt): fd(socket), host(hst), port(prt) { }
};

struct listen_t
{
	std::string host;
	std::string port;

	listen_t(): host(NULL), port("8080") {};
	listen_t(const std::string& hst, const std::string& prt): host(hst), port(prt) {};

	bool operator==(const socket_t& socket) const ;
	bool operator==(const listen_t& ls) const ;
};

std::ostream& operator<<(std::ostream& os, const listen_t& ls);
std::ostream& operator<<(std::ostream& os, const socket_t& socket);

int my_stoi(const std::string& s);
std::vector<std::string> splitIP(const std::string& line, int delim_count, char delim); // tox lrvi

struct NonDigit
{
	bool operator()(char c);
};

#endif // UTIL_HPP