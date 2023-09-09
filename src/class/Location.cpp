#include "Location.hpp"

Location::Location():
	_location_level(1), _cgi(DEFAULT_CGI), _root(), _autoindex(DEFAULT_AUTOINDEX), _httpRedir(HTTP_REDIRECTION),
	_upload_dir(), _indexes(), _allowed_methods(), _parent()
{

}

Location::Location(int loc_l, const std::string& prnt):
	_location_level(loc_l), _parent(prnt)
{

}

void Location::pushIndexes(const std::string& index)
{
	if (std::find(_indexes.begin(), _indexes.end(), index) == _indexes.end())
		_indexes.push_back(index);
}

void Location::pushMethods(const std::string& method)
{
	if (std::find(_allowed_methods.begin(), _allowed_methods.end(), method) == _allowed_methods.end())
		_allowed_methods.push_back(method);
}

void Location::setCgi(const std::string& cgi) { _cgi = cgi; }

void Location::setRoot(const std::string& root)
{
	_root = root[0] == '/' ? root : DEFAULT_ROOT + root;

	if (_root[_root.size() - 1] != '/')
		_root += '/';
}

void Location::setAutoindex(const std::string& autoindex) { _autoindex = autoindex; }

void Location::setHttpRedir(const std::string& redir) { _httpRedir = redir; }

void Location::setUploadDir(const std::string& dir) { _upload_dir = dir[dir.size() - 1] == '/' ? dir : dir + '/'; }

void Location::setLevel(const int& level) { _location_level = level; }

const std::string& Location::getParent() const { return _parent; }

// return values of directives or root by default
const std::string& Location::getValueOf(const std::string& directiveName) const
{
	if (directiveName == "autoindex")
		return _autoindex;
	else if (directiveName == "root")
		return _root;
	else if (directiveName == "return")
		return _httpRedir;
	else if (directiveName == "cgi")
		return _cgi;
	else if (directiveName == "upload_dir")
		return _upload_dir;
	return _root;
}

// return the array indexes (default) or allowed methods
const std::vector<std::string>& Location::getArrayOf(const std::string& directiveName) const
{
	if (directiveName == "index")
		return _indexes;
	else if (directiveName == "allow_methods")
		return _allowed_methods;
	return _indexes;
}

void Location::printEverything() const
{
	std::cout << "\t" << "PARENT:\t\t" << (_parent == "" ? "EMPTY" : _parent) << "\n";
	std::cout << "\t" << "CGI:\t\t" << _cgi << "\n";
	std::cout << "\t" << "ROOT:\t\t" << _root << "\n";
	std::cout << "\t" << "AUTO_INDEX:\t" << _autoindex << "\n";
	std::cout << "\t" << "HTTP_REDIR:\t" << (_httpRedir == "" ? "EMPTY" : _httpRedir)  << "\n";
	std::cout << "\t" << "UPLOAD_DIR:\t" << _upload_dir << "\n";
	std::cout << "\t" << "INDEX_PAGES: \n";
	printVectors(_indexes);
	std::cout << "\t" << "ALLOWED_METHODS: \n";
	printVectors(_allowed_methods);
}

void Location::printVectors(const std::vector<std::string>& vec) const
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << "\t\t" <<  i + 1 << ". " << vec[i] << "\n";
	}
	std::cout << "\n";
}

bool Location::operator<(const Location& other) const
{
	return _location_level < other._location_level;
}

bool Location::operator==(const Location& rhs) const
{
	return _location_level == rhs._location_level && _parent == rhs._parent;
}

Location& Location::operator=(const Location& rhs)
{
	if (_autoindex.size() == 0)
		_autoindex = rhs._autoindex;
	if (_root.size() == 0)
		_root = rhs._root;
	if (_httpRedir.size() == 0)
		_httpRedir = rhs._httpRedir;
	if (_cgi.size() == 0)
		_cgi = rhs._cgi;
	if (_upload_dir.size() == 0)
		_upload_dir = rhs._upload_dir;
	if (_indexes.size() == 0)
		_indexes = rhs._indexes;
	if (_allowed_methods.size() == 0)
		_allowed_methods = rhs._allowed_methods;

	return *this;
}

Location::~Location() { }
