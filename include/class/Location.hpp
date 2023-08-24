#ifndef CLASS_LOCATION_HPP
# define CLASS_LOCATION_HPP

# include <map>
# include <list>
# include <vector>
# include <iostream>

# define DEFAULT_ROOT "www/"
# define HTTP_REDIRECTION ""
# define ALLOWED_METHODS "GET"
# define DEFAULT_AUTOINDEX "off"
# define DEFAULT_INDEX "index.html"
# define DEFAULT_CGI "/usr/bin/python"
# define UPLOAD_DIRECTORY "www/upload/"

class Location
{
private:
	int							_location_level;
	std::string					_cgi;
	std::string					_root;
	std::string					_autoindex;
	std::string					_httpRedir;
	std::string					_upload_dir;
	std::vector<std::string>	_indexes;
	std::vector<std::string>	_allowed_methods;

	std::string					_parent;

public:
	void setCgi(const std::string&);
	void setRoot(const std::string&);
	void pushIndexes(const std::string&);
	void pushMethods(const std::string&);
	void setAutoindex(const std::string&);

	void setLevel(const int& level);
	void setHttpRedir(const std::string&);
	void setUploadDir(const std::string&);

	const std::string& getParent() const ;
	const std::string& getValueOf(const std::string& directiveName) const ;
	const std::vector<std::string>& getArrayOf(const std::string& directiveName) const ;

	void printEverything(const std::string& indent) const ;
	void printVectors(const std::vector<std::string>& vec, const std::string& indent) const ;

public:
	Location();
	Location(int loc_l, const std::string& prnt);

	bool operator<(const Location& other) const ;
	bool operator==(const Location& rhs) const;
	Location& operator=(const Location& rhs);

	~Location();
};

typedef std::map<std::string, Location> LocationMap;

#endif // CLASS_LOCATION_HPP