/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lgrimmei <lgrimmei@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:30 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/12 21:58:19 by lgrimmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "request.hpp"
#include <sstream>
#include <string>

Response::Response(HTTPRequest &request) {
	port = request.port;
	status_code = 0;
	reason_phrase = "";
	maxBodySize = 0;
	full_path = "";
	status_line = "";
	root = "";
	cgiPath = "";
	refer = request.getHeader("Referer");
	content_type = request.getHeader("Content-Type");
	memset(&serverIndex, 0, sizeof(serverIndex));
	serverAutoindex = false;
	http_version = request.version;
	if (http_version == "")
		http_version = "HTTP/1.1";
	method = request.method;
	std::string temp1 = request.getHeader("Host");
	host = whiteSpaceTrim(temp1);
	badRequest = request.badRequest;
	skipBody = 0;
	queryStr = request.queryString;
	std::string temp = request.getHeader("Content-Length");
	if (temp != "")
		requestBodySize = std::atoi(temp.c_str());
	else
		requestBodySize = 0;
	requestBody = request.body;
	contentDisposition = request.getHeader("Content-Disposition");
	boundary = getBoundary(request.getHeader("Content-Type"));
	formData = parseFormData(request.body, boundary);
}

Response::~Response() {
}

void Response::distributer(std::vector<ServerConfig> &servers, std::string uri) {
	std::vector<ServerConfig>::iterator server = findServer(servers);
	location = findLocation(uri, server);
	std::cout << uri << std::endl;
	std::cout << location.path << std::endl;
	if (errorCheck(location) == 1) {
		std::cout << "Request Error detected!" << std::endl;
		std::cout << reason_phrase << std::endl;
		//NULL;
	}
	// else if (uri == "/internal-cgi/get_name.py" || uri == "/internal-cgi/save_name.py") {
	// 	runCgi_internal(uri);
	// }
	else if (method == GET) {
		findFile(uri, server);
	} else if (method == POST) {
		handlePost(uri, server);
	} else if (method == DELETE) {
		handleDelete(uri, server);
	} else {
		status_code = 405;
		reason_phrase = "Method Not Allowed";
	}
	createResponse();
}

void Response::directoryListing(void) {
	body.push_back("<!DOCTYPE html><html><body>");
	body.push_back("<h1>Directory listing</h1>");
	body.push_back("<h2>Index of " + path + "</h2>");
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			body.push_back("<a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a><br>");
		}
		closedir(dir);
	} else {
		perror("opendir");
	}
	body.push_back("</body></html>");
	content_type = "text/html";
}

std::vector<ServerConfig>::iterator	Response::findServer(std::vector<ServerConfig> &servers) {
	std::vector<ServerConfig>::iterator returnServer = servers.end();
	for(std::vector<ServerConfig>::iterator server = servers.begin(); server != servers.end(); ++server) {
		if (returnServer == servers.end() && server->port == port)
		{
			returnServer = server;
			root = server->root;
		}
		std::string hostStr = "";
		if (host != "" && host.find(":") != std::string::npos) {
			hostStr = host.substr(0, host.find(":"));
		} else if (host != "" && host.find(":") == std::string::npos) {
			hostStr = host;
		}
		for (std::vector<std::string>::iterator serverName = server->serverNames.begin(); serverName != server->serverNames.end(); ++serverName) {
			if (hostStr == *serverName && port == server->port) {
				errorMaps = server->errorMaps;
				maxBodySize = server->clientMaxBodySize;
				serverIndex = server->index;
				if (server->autoindex == true) {
					serverAutoindex = true;
				}
				return (server);
			}
		}
	}
	root = returnServer->root;
	maxBodySize = returnServer->clientMaxBodySize;
	errorMaps = returnServer->errorMaps;
	serverIndex = returnServer->index;
	if (returnServer->autoindex == true) {
		serverAutoindex = true;
	}
	return (returnServer);
}

LocationConfig Response::findLocation(std::string uri, std::vector<ServerConfig>::iterator server) {
	for(std::vector<LocationConfig>::iterator location = server->locations.begin(); location != server->locations.end(); ++location) {
		if (uri == location->path) {
			return (*location);
		}
		if (location == server->locations.end() - 1 && uri != "/") {
			size_t pos = uri.rfind('/');
			if (pos != std::string::npos) {
				uri = uri.substr(0, pos);
				location = server->locations.begin() - 1;
			}
		}
	}
	return (server->locations[0]);
}

void Response::findFile(std::string uri, std::vector<ServerConfig>::iterator server) {
	LocationConfig location = findLocation(uri, server);

	for (std::vector<std::string>::iterator method_it = location.methods.begin(); method_it != location.methods.end(); ++method_it) {
		if (*method_it == methodToString(method)) {
			break ;
		}
		else if (method_it == location.methods.end() - 1){
			status_code = 405;
			reason_phrase = "Method Not Allowed";
			header.push_back("Content-Length: 0");
			return ;
		}
	}
	if (location.statusRedir.redirection != "") {
		//std::cout << "redirection: " << location.statusRedir.redirection << std::endl;
		//status_code = 301;
		status_code = location.statusRedir.status_code;
		reason_phrase = "Moved Permanently";
		http_version = "HTTP/1.1";			
		header.push_back("Location: " + location.statusRedir.redirection);
		//header.push_back("Content-Length: 0");
		return ;
	}
	findFileUtil(uri, location);
}

void Response::createBody(void) {
	//std::cout << "Creating Body " << status_code << std::endl;
	if (status_code == 200 && skipBody == 0) {
		std::ifstream file(path.c_str());
		std::string line;
		//std::cout << "File path: " << path.c_str() << std::endl;
		if (file.is_open()) {
			while (getline(file, line)) {
				body.push_back(line);
			}
			file.close();
		}
		else {
			//std::cout << "Unable to open file" << std::endl;
			status_code = 500;
			reason_phrase = "Internal Server Error 2";
			return ;
		}
	}
	else {
		createBodyError();
	}
}

void Response::createBodyError(void) {
	//std::cout << "Creating Body Error " << status_code << std::endl;
	header.push_back("Content-Type: text/html");
	header.push_back("Connection: close");
	header.push_back("Server: webserv");
	std::string statCode = toString(status_code);
	//std::string errorPage = "." + errorMaps[status_code].substr(0, errorMaps[status_code].size() - 1);
	std::string errorPage = "." + errorMaps[status_code];
	std::ifstream file(errorPage.c_str());
	std::string line;
	//std::cout << "Error Page: " << errorPage << std::endl;
	if (file.is_open()) {
		while (getline(file, line)) {
			body.push_back(line);
		}
		file.close();
	}
	else {
		status_code = 500;
		reason_phrase = "Internal Server Error 1";;
	}
}

void Response::createResponse(void) {
	std::stringstream ss;
	if (skipBody == 0)
		createBody();
	status_line = "";
	status_line += http_version;
	status_line += " ";
	ss << status_code;
	status_line += ss.str();
	status_line += " ";
	status_line += reason_phrase;
	//std::cout << status_line << std::endl;
}

void Response::sendResponse(int client_fd) {
	std::string response_string = "";
	response_string += status_line;
	response_string += "\r\n";
	size_t content_length = 0;
	for (std::vector<std::string>::iterator it = body.begin(); it != body.end(); ++it){
		content_length += it->size();
		if (it + 1 != body.end()) {
			content_length += 1;
		}   
	}
	response_string += "Content-Length: ";
	response_string += toString(content_length);

	response_string += "\r\n";
	for (std::vector<std::string>::iterator it = header.begin(); it != header.end(); ++it) {
		response_string += *it;
		response_string += "\r\n";
	}
	response_string += "\r\n";
	for (std::vector<std::string>::iterator it = body.begin(); it != body.end(); ++it){
		response_string += *it;
		if (it + 1 != body.end()) {
			response_string += "\n";
		}
	}
	size_t len = send(client_fd, response_string.c_str(), response_string.size(), 0);
	while (len < response_string.size()) {
		ssize_t sent = send(client_fd, response_string.c_str() + len, response_string.size() - len, 0);
		if (sent == -1) {
			send(client_fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 30, 0);
			break;
		}
		len += sent; 
	}
	//std::cout << "response_string: " << response_string << std::endl;
	
}
