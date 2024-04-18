/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:28:31 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/15 13:12:55 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define MAX_CLIENTS 10000
#define SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 144\r\n\r\n<!DOCTYPE html><html><head><title>ERROR 500</title><meta charset=utf-8><meta http-equiv=X-UA-Compatible content=IE=edge><meta name=viewport content=width=device-width, initial-scale=1><h1>500</h1><h2>Internal server error</h2><p>Sorry, there was an error in processing your request.</p></html>"


struct returnStatus
{
	int			status_code;
	std::string	redirection;
};

struct LocationConfig
{
	std::string					path;
	std::string					root;
	std::vector<std::string>	methods;
	bool						limit_all;
	std::vector<std::string>	index;
	bool						autoindex; // default off
	returnStatus				statusRedir;

//  $Lost = I don't get what this is, so not handling it

	// CGI
	std::vector<std::string>			cgiIndex;
	std::string							cgiExtension;
	std::string							cgiPath;
	std::map<std::string, std::string>	cgiParam;
	// std::string							tryFiles; // $Lost
};

struct ServerConfig
{
	int							port;
	size_t						clientMaxBodySize;
	std::string					host; // not handled in config file parsing but in request parsing
	std::vector<std::string>	index;
	std::string 				root; 
	bool						autoindex;
	std::vector<std::string>	serverNames;
	std::map<int, std::string>	errorMaps;
	std::list<int>				errorKeys;
	std::vector<LocationConfig> locations;
};

// struct LocationConfig {
//     std::string path;
//     std::string root;
//     std::vector<std::string> methods;
//     std::string redirection;
//     bool allowDirectoryListing;
//     std::string indexFile;
//     std::string cgiExtension;
// 	std::string cgiPath;
// 	std::string tryFiles;
// };

// struct ServerConfig {
//     int port;
// 	int clientMaxBodySize;
//     std::string host;
//     std::vector<std::string> serverNames;
//     std::string errorPage404;
//     std::string errorPage50x;
//     std::vector<LocationConfig> locations;
// };

bool separateKeyValue(const std::string& line, std::string& key, std::string& value);
void printConfig(const std::vector<ServerConfig>& servers);
void initconfig(std::vector<ServerConfig>&config);
void errorMapInit(std::vector<ServerConfig> &server);
void logger(std::string log, std::string type);

#endif