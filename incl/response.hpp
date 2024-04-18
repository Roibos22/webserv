/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:38 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/15 17:07:08 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "request.hpp"
#include "webserv.hpp"
#include <cstring>

struct FormData {
	std::string name;
	std::string filename;
	std::string contentType;
	std::string content;
};

class Response {
public:
	int 						status_code;
	int 						maxBodySize;
	int							skipBody;
	int							badRequest;
	int							requestBodySize;
	int							port;
	bool						serverAutoindex;
	LocationConfig 				location;
	std::string 				continue_100;
	std::string 				full_path;
	std::string 				url;
	std::string 				queryStr;
	std::string 				content_type;
	HTTPMethod 					method;
	std::string					status_line;
	std::string 				path;
	std::string					root;
	std::string 				host;
	std::string 				refer;
	std::string 				http_version;
	std::string					reason_phrase;
	std::vector<std::string>	header;
	std::vector<std::string>	body;
	std::vector<std::string>	serverIndex;
	std::map<std::string, std::string>	cgiMap;
	std::string 				cgiPath;
	std::string 				requestBody;
	std::string					fileContent;
	std::string					contentDisposition;	
	std::string					boundary;
	FormData					formData;
	std::map<int, std::string>	errorMaps;
	Response(HTTPRequest &request);
	~Response();

	void	findFile(std::string path, std::vector<ServerConfig>::iterator server);
	void	handlePost(std::string path, std::vector<ServerConfig>::iterator server);
	void	handleUploadFile( void );
	void	deleteFileFromTmpFolder(std::string filename);
	void	handleDelete(std::string path, std::vector<ServerConfig>::iterator server);
	void	createBody(void);
	void	createResponse(void);
	void	sendResponse(int client_fd);
	void	distributer(std::vector<ServerConfig> &servers, std::string uri);
	std::vector<ServerConfig>::iterator	findServer(std::vector<ServerConfig> &servers);
	LocationConfig findLocation(std::string uri, std::vector<ServerConfig>::iterator server);
	void	defineEnv(std::map<std::string, std::string>	&envMap);
	void	directoryListing(void);
	void	runCgi(std::string uri);
	void	handleQueries( void );
	void	handleUndefinedPost( void );
	void	createBodyError(void);
	void findFileUtil(std::string uri, LocationConfig &location);
	int errorCheck(LocationConfig location);
	std::string indexFinder(std::string uri, LocationConfig &location);
	std::string	getBoundary(const std::string& contentType);
	FormData parseFormData(const std::string& requestBody, const std::string& boundary);
	void	executeScript(char **scriptArgs, char *interpreterPath);
	std::string parseContentDispos(void);
	void handleDownload(void);
	void	runCgi_internal(std::string uri);
	int	isValidMethod(std::string uri);
	int getIsValidMethod(std::string uri);
	

};

int isFile(std::string uri);
int isDirectory(std::string uri);
std::string findFileExtension(std::string uri);
std::string contentTypeFinder(std::string path);
std::string methodToString(HTTPMethod method);
int findPort(std::string host);
std::string whiteSpaceTrim(std::string &str);
int isEndingWithExtension(std::string uri, std::string extension);
std::string trimSmeicolon(std::string &str);
void parsePartData(const std::string& part, FormData& formData);
void parseHeaders(const std::string& headers, FormData& formData);
template <typename T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


#endif

// Status line: HTTP-Version SP Status-Code SP Reason-Phrase CRLF -> HTTP/1.1 200 OK
// HTTP Headers: Accept-Ranges, Age, Allow, Cache-Control, Connection, Content-Encoding, Content-Language, Content-Length, Content-Location, Content-MD5, Content-Disposition, Content-Range, Content-Type, Date, ETag, Expires, Last-Modified, Link, Location, P3P, Pragma, Proxy-Authenticate, Refresh, Retry-After, Server, Set-Cookie, Strict-Transport-Security, Trailer, Transfer-Encoding, Vary, Via, Warning, WWW-Authenticate
// Blank line
// Message body

// HTTP/1.1 200 OK
// Date: Mon, 27 Jul 2009 12:28:53 GMT
// Server: Apache/2.2.14 (Win32)
// Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT
// Content-Length: 88
// Content-Type: text/html
// Connection: Closed

// <html>
// <body>
// <h1>Hello, World!</h1>
// </body>
// </html>

