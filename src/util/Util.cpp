#include "Util.hpp"

std::ostream& operator<<(std::ostream& os, const listen_t& ls)
{
	os << ls.host << " " << ls.port;
	return os;
}

std::ostream& operator<<(std::ostream& os, const socket_t& socket)
{
	os << socket.fd << ", host: " << socket.host << ", port: " << socket.port;
	return os;
}

bool NonDigit::operator()(char c)
{
	return !::isdigit(c);
}

std::vector<std::string> splitIP(const std::string& line, int delim_count, char delim) // tox lrvi
{
	std::stringstream ss(line);
	std::vector<std::string> ret;

	for (int i = 0; i < delim_count + 1; ++i)
	{
		std::string res;

		std::getline(ss, res, delim);

		ret.push_back(res);
	}

	return ret;
}

std::string my_to_string(int num)
{
	size_t pos = 0;
	std::string ret;

	if (num == 0)
		return ("0");

	if (num < 0)
	{
		ret.insert(0, 1, '-');
		++pos;
	}

	while (num)
	{
		ret.insert(pos, 1, (num % 10) + 48);
		num /= 10;
	}

	return ret;
}

// only positive numbers, -1 on error
int my_stoi(const std::string& s)
{
	if (s.size() > 10 || (s.size() == 10 && s > "2147483647")
		|| std::find_if(s.begin(), s.end(), NonDigit()) != s.end())
		return (-1);
	
	int res = 0;

	for (size_t i = 0; i < s.size(); ++i)
	{
		res *= 10;
		res += s[i] - 48;
	}

	return res;
}

bool socket_t::operator==(const socket_t& lhs) const { return host == lhs.host && port == lhs.port; }
bool socket_t::operator==(const listen_t& lhs) const { return host == lhs.host && port == lhs.port; }
bool socket_t::operator==(const int& socket) const { return fd == socket; }
bool listen_t::operator==(const socket_t& socket) const { return host == socket.host && port == socket.port; }
bool socket_t::operator==(const std::string& lhs) const { return port == lhs; }
bool listen_t::operator==(const listen_t& ls) const { return host == ls.host && port == ls.port; }
