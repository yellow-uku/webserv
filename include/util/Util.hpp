#ifndef UTIL_HPP
# define UTIL_HPP

# include <vector>
# include <sstream>
# include <iostream>
# include <algorithm>

typedef struct listen_s
{
	std::string host;
	std::string port;

	listen_s(): host(NULL), port("8080") {};
	listen_s(const std::string& hst, const std::string& prt): host(hst), port(prt) {};
	bool operator==(const listen_s& ls) const;

} listen_t ;

typedef struct socket
{
	int fd;
	std::string host;
	std::string port;

	bool operator==(const socket& lhs) const { return host == lhs.host && port == lhs.port; }
	bool operator==(const listen_t& lhs) const { return host == lhs.host && port == lhs.port; }
	bool operator==(const int& socket) const { return fd == socket; }

	socket(): fd(-1) { }
	socket(const int& socket, const std::string& hst, const std::string& prt): fd(socket), host(hst), port(prt) { }
} socket_t;

std::ostream& operator<<(std::ostream& os, const listen_t& ls);
std::ostream& operator<<(std::ostream& os, const socket_t& socket);

int my_stoi(const std::string& s);
std::vector<std::string> splitIP(const std::string& line, int delim_count, char delim); // tox lrvi

struct NonDigit
{
	bool operator()(char c);
};

#endif // UTIL_HPP