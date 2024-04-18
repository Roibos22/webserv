/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lgrimmei <lgrimmei@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/04/18 15:56:01 by lgrimmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "configParser.hpp"
#include "response.hpp"
#include <string>

using std::cerr;
using std::cout;
using std::endl;

Utils	ut2;

configParser::configParser()
{}

configParser::configParser(std::string &file)
: Lexir(file)
{
	this->init();
}

configParser::configParser(std::string &file, char **env)
: Lexir(file)
{
	this->env = env;
	this->init();
}

configParser::~configParser()
{}

/*				Error 			*/

int		configParser::validErrorValue(int num, const std::string& s, ServerConfig *currentServer)
{
	for(std::map<int, std::string>::iterator i = currentServer->errorMaps.begin(); i != currentServer->errorMaps.end(); i++)
	{
		if (currentServer->errorMaps.find(num) != currentServer->errorMaps.end())
			throw (this->def("Error code found twice:\n" + s));
	}
	if((num >= 400 && num <= 418) || (num >= 421 && num <= 426) || num == 428 || num == 429 || num == 431 || num == 451 || (num >= 500 && num <= 511)
		|| num == 444 || (num >= 494 && num <= 497) || num == 499)
		return (0);
	return (1);
}

// Parsing

void	configParser::addErrorPage(const std::string& s, ServerConfig *currentServer)
{
	std::stringstream	ss(s);
	std::string			error_page;
	std::string			fileAddr;
	std::string			temp;
	int					errorValue;

	ss >> error_page;
	ss >> temp;
	for (int i = 0; temp[i]; i++)
	{
		if(std::isdigit(temp[i]) == 0)
			throw (LineNotValid(s));
	}
	std::stringstream	num(temp);
	num >> errorValue;
	if(this->validErrorValue(errorValue, s, currentServer))
		throw this->def("Error code is not accepted by this webserver:\n" + s);
	ss >> fileAddr;
	ut2.trimSpacesAndFinalSemiColon(fileAddr);
	size_t	pos;
	if(ut2.countDotDotSlash(s, pos) > 0)
		fileAddr = fileAddr.substr(pos, fileAddr.length() - pos);
	if(*fileAddr.begin() != '/' && *fileAddr.begin() != '.')
	{
		fileAddr = "/" + fileAddr;
	}
	currentServer->errorKeys.push_back(errorValue);
	currentServer->errorMaps[errorValue] = fileAddr;
}

void	configParser::addListenPort(std::string& s, ServerConfig *currentServer)
{
	std::stringstream	ss(s);
	int	num;
	int check = currentServer->port;

	while(s.empty() == false && std::isspace(*s.begin()))
		s.erase(s.begin());
	if(s.empty() == true)
		throw this->LineNotValid("Listen line missing Port value: " + s);
	for (int i = 0; s[i] && s[i] != ';'; i++)
	{
		if(!std::isdigit(s[i]))
			throw this->LineNotValid("Line not valid: " + s);
	}
	ss << s;
	ss >> num;
	if (num < 0 || num > 65535)
		throw this->LineNotValid("Port number not valid: " + s);
	if (check == num || check != -5)
		throw this->LineNotValid("Port number found twice in one server block: " + s);
	currentServer->port = num;
}

void	configParser::addMaxBodySize(std::string& s, ServerConfig *currentServer, std::string line)
{
	std::stringstream	ss(s);
	size_t	num;

	if(ut2.count_words(s) == 0 || ut2.count_words(s) > 2)
		this->LineNotValid(line);
	ss >> num;
	// conv << num % 10;
	// conv >> lastDigitChar;
	// int i = s.find_last_of(lastDigitChar) +1;
	std::string unit;
	ss >> unit;
	ut2.trimSpacesAndFinalSemiColon(unit);
	if(unit.empty() == true)
		unit = "KB";
	else if (unit == "B" || unit == "b")
		num = num * 1;
	else if(unit == "K" || unit == "KB" || unit == "k" || unit == "kb")
		num = num * 1024;
	else if(unit == "M" || unit == "MB" || unit == "m" || unit == "mb")
		num = num * 1024 * 1024;
	else if(unit == "G" || unit == "GB" || unit == "g" || unit == "GB")
		num = num * 1024 * 1024;
	else
		throw this->def("Unit of Max Body Size not looking good:" + s + "\nvalid units are B, KB, MB, GB, followed by a ';'\n");
	currentServer->clientMaxBodySize = num;
}

int	configParser::findTypeStart(std::string type, int start)
{
	int	lines = 1;

	for(std::list<std::string>::iterator i = this->fileContent.begin(); i != this->fileContent.end(); i++, lines++)
	{
		if(lines < start)
			continue ;
		if(i->compare(0, type.length(), type) == 0)
		{
			while(i != this->fileContent.end())
			{
				if(i->find_first_of('{', 0))
					return (lines);
				i++;
			}
			return (-1);
		}
	}
	return (-1);
}

int	configParser::findTypeEnd(int start, std::string type)
{
	(void) type;
	int		lines = 1;
	int		open = 0;
	int		close = 0;
	size_t	openIndex;
	size_t	closedIndex;
	std::list<std::string>::iterator stored;

	if(start < 1)
		return (-1);
	for(std::list<std::string>::iterator i = this->fileContent.begin(); i != this->fileContent.end(); i++, lines++)
	{
		openIndex = 0;
		closedIndex = 0;
		if(lines < start)
			continue ;
		stored = i;
		while(1)
		{
			openIndex = i->find('{', openIndex++);
			if(openIndex == std::string::npos || openIndex < 0)
				break ;
			openIndex++;
			open++;
		}
		while(1)
		{
			closedIndex = i->find('}', closedIndex);
			if(closedIndex == std::string::npos || closedIndex < 0)
				break ;
			closedIndex++;
			close++;
		}
		if(open > 0 && open == close)
			return (lines);
	}
	return (-1);
}

std::string	configParser::extractValue(std::string buffer, std::string name)
{
	std::string	ret;
	ut2.trimSpacesAndFinalSemiColon(buffer);
	if(ut2.count_words(buffer) != 2)
		throw (this->LineNotValid(buffer));
	ret = buffer.substr(name.length(), buffer.length());
	if(ret.empty() == true)
		throw this->def("server_name value is empty\n");
	return (ret);
}

void	configParser::parseNames(std::string line, ServerConfig *currentServer)
{
	ut2.trimSpacesAndFinalSemiColon(line);
	std::stringstream	ss(line);
	std::string	name;

	ss >> name;
	while(name.empty() == false)
	{
		currentServer->serverNames.push_back(name);
		ut2.outputVector(currentServer->serverNames);
		name.clear();
		ss >> name;
	}
}

void	configParser::addMethods(std::string s, LocationConfig& currLocation, std::string line)
{
	while(currLocation.methods.empty() != true)
		currLocation.methods.pop_back();
	std::stringstream	ss(s);
	std::string			method;

	ss >> method;
	ut2.trimSpacesAndFinalSemiColon(method);
	if(method.empty())
	{
		currLocation.limit_all = true;
		return ;
	}
	currLocation.limit_all = false;
	while(method.empty() == false)
	{
		if(method != "GET" && method != "POST" && method != "DELETE")
			throw this->def("Method is not valid: " + method + "\n" + "Please verify this line: " + line + "\n");
		currLocation.methods.push_back(method);
		method.clear();
		ss >> method;
		ut2.trimSpacesAndFinalSemiColon(method);
	}
	if(ut2.checkDoublesVectorString(currLocation.methods))
		throw (this->LineNotValid("Found double methods in here: " + line + "\n"));
}

void	configParser::addRoot(std::string s, LocationConfig& currLocation, std::string line)
{
	ut2.trimSpacesAndFinalSemiColon(s);
	if(ut2.count_words(s) != 1)
		throw (this->LineNotValid(line));
	ut2.trimDoubleQuotes(s);
	size_t	pos;
	if(ut2.countDotDotSlash(s, pos) > 0)
		s = s.substr(pos, s.length() - pos);
	if(*s.begin() == '/')
		currLocation.root = s;
	else
	{
		currLocation.root += "/";
		currLocation.root += s;
	}
}

std::string	configParser::addRootAll(std::string s, std::string line)
{
	std::string	ret;

	ut2.trimSpacesAndFinalSemiColon(s);
	if(ut2.count_words(s) != 1)
		throw (this->LineNotValid(line));
	ut2.trimDoubleQuotes(s);
	size_t	pos;
	if(s.substr(0, 3) == "../")
	{
		ut2.countDotDotSlash(s, pos);
		s = s.substr(pos, s.length() - pos);
	}
	if(*s.begin() == '/')
		ret = s;
	else
	{
		ret += "/";
		ret += s;
	}
	return(ret);
}

void	configParser::addPath(std::string s, LocationConfig& currLocation, std::string line)
{
	ut2.trimSpacesAndFinalBracket(s);
	ut2.splitWordsTrimSpaces(s);
	int	wordCount = ut2.count_words(s);
	if(wordCount == 0)
		throw (this->def("Path to location not present: " + line + "\n"));
	if(wordCount > 1)
		throw (this->def("Multiple paths to location found, FIX THIS!!!\n" + line + "\n"));
	ut2.trimSpaces(s);
	currLocation.path = s;
}

void	configParser::addStatusRedir(std::string s, LocationConfig& currLocation, std::string line)
{
	ut2.trimSpacesAndFinalSemiColon(s);
	ut2.splitWordsTrimSpaces(s);
	int	wordCount = ut2.count_words(s);
	if(wordCount == 0 || wordCount > 2)
		throw (this->LineNotValid(line));
	std::stringstream	ss(s);
	std::string dummyString;
	ss >> dummyString;
	std::stringstream	finalss(s);
	if(wordCount == 1 && std::isdigit(dummyString[0]) == false && std::isdigit(dummyString[1]) == false && std::isdigit(dummyString[2]) == false)
		currLocation.statusRedir.status_code = 301;
	if(dummyString.length() == 3 && std::isdigit(dummyString[0]) == true && std::isdigit(dummyString[1]) == true && std::isdigit(dummyString[2]) == true)
	{
		finalss >> currLocation.statusRedir.status_code;
		finalss >> currLocation.statusRedir.redirection;
	}
	else
	{
		finalss >> currLocation.statusRedir.redirection;
		finalss >> currLocation.statusRedir.status_code;
	}
}

std::vector<std::string>	configParser::addCgiIndex(std::string s, std::string line, LocationConfig *currentLocation)
{
	std::vector<std::string> ret;

	ut2.trimSpacesAndFinalSemiColon(s);
	if(s.empty() == true)
		throw this->LineNotValid(line);
	ret = ut2.splitWordsTrimSpaces(s);

	if (ut2.checkDoublesVectorString(ret))
		throw this->LineNotValid(line);
	for(std::vector<std::string>::iterator it = ret.begin(); it < ret.end(); it++)
	{
		if(ut2.checkFileType(*it, ".php") != true && ut2.checkFileType(*it, ".py") != true)
			throw this->LineNotValid(line);
		currentLocation->cgiIndex.push_back(*it);
	}
	return (ret);
}

std::vector<std::string>	configParser::addIndex(std::string s, std::string line, LocationConfig *currentLocation)
{
	std::vector<std::string> ret;

	ut2.trimSpacesAndFinalSemiColon(s);
	if(s.empty() == true)
		throw this->LineNotValid(line);
	ret = ut2.splitWordsTrimSpaces(s);
	if (ut2.checkDoublesVectorString(ret))
		throw this->LineNotValid(line);
	for(std::vector<std::string>::iterator it = ret.begin(); it < ret.end(); it++)
	{
		currentLocation->index.push_back(*it);
	}
	return (ret);
}

std::vector<std::string>	configParser::addServerIndex(std::string s, std::string line, ServerConfig *currentServer)
{
	std::vector<std::string> ret;

	ut2.trimSpacesAndFinalSemiColon(s);
	if(s.empty() == true)
		throw this->LineNotValid(line);
	ret = ut2.splitWordsTrimSpaces(s);

	if (ut2.checkDoublesVectorString(ret))
		throw this->LineNotValid(line);
	for(std::vector<std::string>::iterator it = ret.begin(); it < ret.end(); it++)
	{
		currentServer->index.push_back(*it);
	}
	return (ret);
}

bool	configParser::addAutoIndex(std::string s, std::string line)
{
	ut2.trimSpacesAndFinalSemiColon(s);
	if(s.empty() == true)
		throw this->LineNotValid(line);
	if(s == "off" || s == "0")
		return (false);
	if(s == "on" || s == "1")
		return (true);
	throw this->LineNotValid(line);
}

// CGI

void	configParser::addCgiParam(std::string s, LocationConfig& currLocation, std::string line)
{
	std::vector<std::string> paramList;
	paramList.push_back("QUERY_STRING");
	paramList.push_back("REQUEST_METHOD");
	paramList.push_back("CONTENT_TYPE");
	paramList.push_back("CONTENT_LENGTH");
	paramList.push_back("SCRIPT_FILENAME");
	paramList.push_back("SCRIPT_NAME");
	paramList.push_back("PATH_INFO");
	paramList.push_back("PATH_TRANSLATED");
	paramList.push_back("REQUEST_URI");
	paramList.push_back("DOCUMENT_URI");
	paramList.push_back("DOCUMENT_ROOT");
	paramList.push_back("SERVER_PROTOCOL");
	paramList.push_back("GATEWAY_INTERFACE");
	paramList.push_back("SERVER_SOFTWARE");
	paramList.push_back("REMOTE_ADDR");
	paramList.push_back("REMOTE_PORT");
	paramList.push_back("SERVER_ADDR");
	paramList.push_back("SERVER_PORT");
	paramList.push_back("SERVER_NAME");
	paramList.push_back("CGI_EXTENSION");
	paramList.push_back("HTTPS");

	ut2.trimSpacesAndFinalBracket(s);
	if(ut2.count_words(s) != 2)
		throw (this->def("cgi param format not good, requires param type and param value:\n" + line));
	std::stringstream	ss(s);
	std::string			param;
	std::string			value;
	ss >> param;
	ss >> value;
	for(std::map<std::string, std::string>::iterator it = currLocation.cgiParam.begin(); it != currLocation.cgiParam.end(); it++)
		if(it->first == param)
			throw (this->def("This cgi param was already parsed above, no param doubles plzzzzzz !!!:\n" + line));
	std::vector<std::string>::iterator it;
	for(it = paramList.begin(); it != paramList.end(); it++)
	{
		if(*it == param)
			break;
	}
	if(it == paramList.end())
		throw (ut2.outputVector(paramList), cout << "\n", this->LineNotValid(line + "\nThis param type is not supported, the available params are listed above"));
	currLocation.cgiParam[param] = value;
}

void	configParser::addCgiToEnv(LocationConfig *currentLocation)
{
	for(std::map<std::string, std::string>::iterator it = currentLocation->cgiParam.begin(); it != currentLocation->cgiParam.end(); it++)
	{
		setenv(it->first.c_str(), it->second.c_str(), 0);
	}
}

void	configParser::addLocation(std::list<std::string>::iterator& i, std::list<std::string>::iterator end, int& lines, ServerConfig& currentServer)
{
	int	locStart = lines;
	int	locEnd = 0;

	LocationConfig	currLocation;
	locStart = this->findTypeStart("location", locStart);
	locEnd = this->findTypeEnd(locStart, "location");
	if(locStart < 0 || locEnd < 0)
		this->def("Problem with the location brackets {}:\n" + *i + "\n");
	currLocation =  this->defaultLocation(&currentServer);
	this->addPath(&(*i)[9], currLocation, *i); // get path before brackets
	while(i != end && lines >= locStart && lines <= locEnd)
	{
		if(i->compare(0, 13, "limit_except ") == 0)
			this->addMethods(&(*i)[13], currLocation, *i);
		else if(!i->compare(0, 5, "root "))
			this->addRoot(&(*i)[5], currLocation, *i);
		else if(!i->compare(0, 7, "return "))
			this->addStatusRedir(&(*i)[7], currLocation, *i);
		else if(!i->compare(0, 6, "index "))
			this->addIndex(&(*i)[6], *i, &currLocation);
		else if(!i->compare(0, 10, "autoindex "))
			currLocation.autoindex = this->addAutoIndex(&(*i)[10], *i);
		else if(!i->compare(0, 13, "fastcgi_index "))
			addCgiIndex(&(*i)[13], *i, &currLocation);
		else if(!i->compare(0, 13, "fastcgi_pass "))
		{
			if(currLocation.cgiPath.empty() == false)
				throw (this->def("Can not have fastcgi_pass twice in one location\n" + *i));
			currLocation.cgiPath = this->extractValue(*i, "fastcgi_pass ");
			if (ut2.checkFileType(currLocation.cgiPath, ".py") == true)
				currLocation.cgiExtension = ".py";
			else
				throw (this->def("We don't handle this cgi file type, we only do \".py\", we keep it minimalistic:\n" + *i));
		}
		else if(!i->compare(0, 13, "fastcgi_param"))
			this->addCgiParam(&(*i)[14], currLocation, *i);
		lines++;
		i++;
	}
	if(currLocation.limit_all == false && currLocation.methods.empty() == true)
	{
		currLocation.methods.push_back("GET");
		currLocation.methods.push_back("POST");
		currLocation.methods.push_back("DELETE");
	}
	this->addCgiToEnv(&currLocation); // addCgiParam to environment variables
	if(currLocation.index.empty())
	{
		for(std::vector<std::string>::iterator it = currentServer.index.begin(); it != currentServer.index.end(); it++)
			currLocation.index.push_back(*it);
	}
	lines--;
	i--;
	currentServer.locations.push_back(currLocation);
}

ServerConfig	configParser::defaultServer(void)
{
	ServerConfig	def;

	def.clientMaxBodySize = 5 * 1024 * 1024;
	def.autoindex = false;
	def.port = -5;
	return(def);
};

LocationConfig	configParser::defaultLocation(ServerConfig *currentServer)
{
	LocationConfig	def;

	int len = ut2.containerLength(this->SC);
	if(len >= 1)
	{
		def.index = currentServer->index;
		def.autoindex = currentServer->autoindex;
	}
	if(len == 0)
		def.autoindex = false;
	def.limit_all = false;
	def.statusRedir.status_code = 301;
	return(def);
}

void			configParser::fillUpErrorMaps(ServerConfig* currentServer)
{
	bool	found;

	for(std::map<int, std::string>::iterator it = this->defaultErrorMaps.begin(); it != this->defaultErrorMaps.end(); it++)
	{
		found = false;
		for(std::map<int, std::string>::iterator is = currentServer->errorMaps.begin(); is != currentServer->errorMaps.end(); is++)
		{
			if(it->first == is->first)
				found = true;
		}
		if(found == false)
		{
			currentServer->errorMaps.insert(std::make_pair(it->first, it->second));
			currentServer->errorKeys.push_back(it->first);
		}
	}
}

std::list<std::string>::iterator	configParser::initServer(std::list<std::string>::iterator i, std::list<std::string>::iterator end, int& lines)
{
	ServerConfig	currentServer;
	int	serverStart = lines;
	int	serverEnd = 0;

	serverStart = this->findTypeStart("server", serverStart);
	serverEnd = this->findTypeEnd(serverStart, "server");
	if(serverStart < 1 || serverEnd < 1)
		this->def("Problem with the server brackets {}:\n" + *i + "\n");
	currentServer = this->defaultServer();
	//cout << "server parse start\n";
	while(i != end && lines >= serverStart && lines <= serverEnd)
	{
		if(ut2.count_words(*i) <= 1 && ut2.strNeedsNoSemiColon(*i) == false)
			throw (this->LineNotValid(*i + "\nline: " + ut2.intToString(lines) + "-> it's likely missing a value\n"));
		if (i->compare(0, 5, "host ") == 0)
			currentServer.host = this->extractValue(*i, "host");
		else if(i->compare(0, 7, "listen ") == 0 || i->compare(0, 5, "port ") == 0)
		{
			std::string str;
			if(i->compare(0, 7, "listen ") == 0)
				str = (&(*i)[7]);
			else
				str = (&(*i)[5]);
			this->addListenPort(str, &currentServer);
		}
		else if(i->compare(0, 21, "client_max_body_size ") == 0 || i->compare(0, 13, "cli_max_size ") == 0)
		{
			std::string str = &(*i)[21];
			this->addMaxBodySize(str, &currentServer, *i);
		}
		else if(i->compare(0, 11, "error_page ") == 0)
			this->addErrorPage(*i, &currentServer);
		else if (i->compare(0, 12, "server_name ") == 0)
			this->parseNames(&(*i)[12], &currentServer);
		else if(!i->compare(0, 6, "index "))
			currentServer.index = this->addServerIndex(&(*i)[6], *i, &currentServer);
		else if(!i->compare(0, 10, "autoindex "))
			currentServer.autoindex = this->addAutoIndex(&(*i)[10], *i);
		else if(!i->compare(0, 5, "root "))
			currentServer.root = this->addRootAll(&(*i)[5], *i);
		else if (i->compare(0, 8, "location") == 0)
			this->addLocation(i, end, lines, currentServer);
		lines++;
		i++;
	}
	//cout << "server parse done\n";
	if(currentServer.serverNames.empty() == true)
		currentServer.serverNames.push_back("default");
	this->fillUpErrorMaps(&currentServer);
	lines--;
	i--;
	this->SC.push_back(currentServer);
	return (i);
}

std::string	get_cmd_path(std::list<std::string> splitPaths, char *cmmd)
{
	std::string	path;

	for (std::list<std::string>::iterator it = splitPaths.begin(); it != splitPaths.end(); it++)
	{
		path = *it;
		path += "/";
		path += cmmd;
		if (access(path.c_str(), F_OK) == 0)
			return ((char *)path.c_str());
	}
	return (NULL);
}

std::string	find_path(char **env)
{
	int	i;
	std::string	path = "PATH";
	std::string	temp;

	i = 0;
	while (env[i] != NULL)
	{
		temp = env[i];
		if(temp.substr(0, 4) == path)
			return (&env[i][5]);
		i++;
	}
	return (NULL);
}

void	Child(int *pipefd, char**env, char *path, char **arg)
{
	if(chdir("./test_websites/default_error_pages") < 0)
	{
		perror ("failed to chdir to retrieve error pages");
		exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	if(dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		perror ("failed to link STDOUT_FILENO to pipe when initializing default error files");
		exit(EXIT_FAILURE);
	}
	int error = execve (path, arg, env);
	if (error == -1)
	{
		perror ("failed to execute ls to get error pages");
		exit(EXIT_FAILURE);
		close(pipefd[1]);
	}
}

std::string	readFd(int fd)
{
	int			bufferSize = 1280;
	char		buff[bufferSize];
	std::string	ret;
	ssize_t		br;

	while(1)
	{
		br = read(fd, buff, bufferSize -1);
		buff[br] = 0;
		ret += buff;
		if(br < bufferSize -1)
			break ;
	}
	return (ret);
}

void	configParser::addDefaultErrorPage(std::string s)
{
	ut2.trimSpaces(s);
	if(ut2.count_words(s) != 1 || s.length() != 8 || ut2.checkFileType(s, ".html") == false)
		return ;
	if(std::isdigit(s[0]) == false || std::isdigit(s[1]) == false || std::isdigit(s[2]) == false)
		return ;
	std::istringstream ss(s.substr(0, 3));
	int	value;
	ss >> value;
	this->defaultErrorMaps.insert(std::make_pair(value, s));
}

void	configParser::initDefaultMaps()
{
	int		pipefd[2];
	pid_t	pid;

	std::string	path = find_path(this->env); // find path in env variables
	std::list<std::string> splitPaths = ut2.listSplitStrings(path, ':'); // split path into list of strings
	path = get_cmd_path(splitPaths, (char *)"ls"); // path to ls cmd
	char* arg[] = {(char*)path.c_str(), NULL};
	if(pipe(pipefd) < 0)
		throw this->def("Error creating pipe\n");
	pid = fork();
	if(pid < 0)
	{
		perror("pid");
		exit(EXIT_FAILURE);
	}
	if(pid == 0)
		Child(pipefd, this->env, (char *)path.c_str(), arg);
	waitpid(pid, NULL, 0);
	std::string	read = readFd(pipefd[0]);
	close(pipefd[1]);
	close(pipefd[0]);
	std::list<std::string> readList = ut2.listSplitStrings(read, '\n');
	for (std::list<std::string>::iterator it = readList.begin(); it != readList.end(); it++)
	{
		this->addDefaultErrorPage(*it);
	}
	// for (std::map<int, std::string>::iterator it = this->defaultErrorMaps.begin(); it != this->defaultErrorMaps.end(); it++)
	// 	cout << "def error map first: " << it->first << endl << "second: " << it->second << endl;
}

void	configParser::init()
{
	int	line = 1;
	
	int	httpStart = this->findTypeStart("http", 0);
	int	httpEnd = this->findTypeEnd(httpStart, "http");
	this->initDefaultMaps();
	if(httpStart < 1 || httpEnd < 1)
		throw this->def("Couldn't find http starting or end point in file, verify your brackets {}\n");
	for(std::list<std::string>::iterator i = this->fileContent.begin(); i != this->fileContent.end(); i++, line++)
	{
		if(line >= httpStart && line <= httpEnd)
		{
			if(i->compare(0, 6, "server") == 0)
				i = this->initServer(i, this->fileContent.end(), line);
		}
	}
}

std::list<ServerConfig>& configParser::getServerList()
{
	return (this->SC);
}

// TESTING - OUTPUT

void	configParser::outputLocation(LocationConfig& curr)
{
	cout << endl;
	cout << "LOCATION\n";
	cout << "path : " << curr.path << endl;
	cout << "root : " << curr.root << endl;
	cout << "return status : " << curr.statusRedir.status_code << endl << "return redir : " << curr.statusRedir.redirection << endl;
	cout << "METHODS: ";
	for (std::vector<std::string>::iterator it = curr.methods.begin(); it != curr.methods.end(); it++)
		cout << *it << " ";
	cout << endl;
	if(curr.index.empty() == false)
	{
		cout << "Location indexes : ";
		ut2.outputVector(curr.index);
		cout << endl;
	}
	if(curr.autoindex == true)
		cout << "autoindex: on" << endl;
	if(curr.autoindex == false)
		cout << "autoindex: off" << endl;
	cout << "CGI param map:\n";
	ut2.outputMap(curr.cgiParam);
	cout << endl;
	cout << "CGI path: " << curr.cgiPath << endl;
}

void	configParser::outputServer()
{
	for(std::list<ServerConfig>::iterator it = this->SC.begin(); it != this->SC.end(); it++)
	{
		cout << "\n";
		cout << "Server names:\n";
		for (std::vector<std::string>::iterator names_it = it->serverNames.begin(); names_it != it->serverNames.end(); names_it++)
			cout << *names_it << endl;
		cout << "\n";
		cout << "host : " << it->host << endl; 
		std::cout << "port : " << it->port << endl;
		std::cout << "clientMaxBodySize : " << it->clientMaxBodySize << " bytes" << endl;
		if(it->index.empty() == false)
		{
			cout << "Server indexes : ";
			ut2.outputVector(it->index);
			cout << endl;
		}
		cout << "ERROR MAP\n";
		for (std::map<int, std::string>::iterator maps = it->errorMaps.begin(); maps != it->errorMaps.end(); maps++)
			cout << "keycode: " << maps->first << " value: " << maps->second << endl;
		cout << "ERROR KEYS\n";
		for (std::list<int>::iterator keys = it->errorKeys.begin(); keys != it->errorKeys.end(); keys++)
			cout << "stored key: " << *keys << endl;
		cout << endl;
		if(it->autoindex == true)
				cout << "autoindex: on" << endl;
		else if(it->autoindex == false)
				cout << "autoindex: off" << endl;
		for(std::vector<LocationConfig>::iterator loc_it = it->locations.begin(); loc_it != it->locations.end(); loc_it++)
			outputLocation(*loc_it);
	}
	cout << endl;
}

void	configParser::outputParse(void)
{
	cout << "\nOUTPUT PARSE:\n";
	this->outputServer();
}
