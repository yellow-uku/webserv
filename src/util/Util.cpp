#include "Util.hpp"

std::ostream& operator<<(std::ostream& os, const listen_t& ls)
{
	os << ls.host << " " << ls.port;
	return os;
}

bool listen_t::operator==(const listen_s& ls)
{
	return host == ls.host && port == ls.port;
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
