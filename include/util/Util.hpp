#ifndef UTIL_HPP
# define UTIL_HPP

# include <vector>
# include <sstream>
# include <iostream>
# include <algorithm>

# include <unistd.h>
# include <stdint.h>

struct listen_t;

struct socket_t
{
	int fd;
	time_t timeout;
	std::string host;
	std::string port;

	bool operator==(const int& socket) const ;
	bool operator==(const socket_t& lhs) const ;
	bool operator==(const listen_t& lhs) const ;
	bool operator==(const std::string& lhs) const ;

	socket_t(): fd(-1) { }
	socket_t(const std::string& hst, const std::string& prt): fd(-1), timeout(time(NULL)), host(hst), port(prt) { }
	socket_t(const int& socket, const std::string& hst, const std::string& prt): fd(socket), timeout(time(NULL)), host(hst), port(prt) { }
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

int							my_stoi(const std::string& s);
bool						contains(char const * const allowed[], const std::string &token);
void						close_pipes(int pipe_from[2], int pipe_to[2]);
ssize_t						my_stos_t(const std::string& s);
ssize_t						hex_to_int(const std::string& hex);
std::string					my_to_string(ssize_t num);
std::vector<std::string>	splitIP(const std::string& line, int delim_count, char delim); // tox lrvi

struct NonDigit
{
	bool operator()(char c);
};

struct IsNotSpace
{
	bool operator()(char c);
};

struct NotValidHex
{
	bool operator()(char c);
};

#endif // UTIL_HPP
