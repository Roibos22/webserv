#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "Lexir.hpp"
#include "Utils.hpp"
#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include <iostream>
#include "../webserv.hpp"
#include <unistd.h>
#include <stdlib.h>

class configParser : public Lexir
{
	public:
		configParser();
		configParser(std::string &file);
		configParser(std::string &file, char **env);
		~configParser();
		void	init();

		// Server
		ServerConfig	defaultServer(void);
		std::list<std::string>::iterator	initServer(std::list<std::string>::iterator i, std::list<std::string>::iterator end, int& lines);
		void			addErrorPage(const std::string& s, ServerConfig *currentServer);
		void			addDefaultErrorPage(std::string s);
		void			addListenPort(std::string& s, ServerConfig *currentServer);
		void			addMaxBodySize(std::string& s, ServerConfig *currentServer, std::string line);
		int				validErrorValue(int num, const std::string& s, ServerConfig *currentServer);
		std::string		extractValue(std::string buffer, std::string name);
		void			parseNames(std::string line, ServerConfig *currentServer);
		std::string		addRootAll(std::string s, std::string line);
		void			fillUpErrorMaps(ServerConfig* currentServer);
		void			initDefaultMaps();
		std::vector<std::string>	addServerIndex(std::string s, std::string line, ServerConfig *currentServer);

		// Location
		LocationConfig	defaultLocation(ServerConfig *currentServer);
		void			addLocation(std::list<std::string>::iterator& i, std::list<std::string>::iterator end, int& lines,ServerConfig& currentServer);
		void			addMethods(std::string s, LocationConfig& currLocation, std::string line);
		void			addRoot(std::string s, LocationConfig& currLocation, std::string line);
		void			addPath(std::string s, LocationConfig& currLocation, std::string line);
		void			addStatusRedir(std::string s, LocationConfig& currLocation, std::string line);

		// CGI
		void	addCgiParam(std::string s, LocationConfig& currLocation, std::string line);
		std::vector<std::string>	addCgiIndex(std::string s, std::string line, LocationConfig *currentLocation);
		void	addCgiToEnv(LocationConfig *currentLocation);

		std::vector<std::string>	addIndex(std::string s, std::string line, LocationConfig *currentLocation);
		bool						addAutoIndex(std::string s, std::string line);

		// SETTERS
		int		findTypeStart(std::string type, int start);
		int		findTypeEnd(int start, std::string type);

		// Getter
		std::list<ServerConfig>& getServerList();

		//Output Debugger
		void	outputParse(void);
		void	outputServer();
		void	outputLocation(LocationConfig& curr);
		char**	env;

	private:
		// PARSE VALUES FROM FILE
		std::string					serverName;
		std::map<int, std::string>	defaultErrorMaps;
		std::list<ServerConfig>		SC;
};

#endif
