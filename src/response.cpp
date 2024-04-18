/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:30 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/15 21:06:42 by ashojach         ###   ########.fr       */
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
	if (request.getHeader("Expect") == "100-continue") {
		continue_100 = "100-continue";
		return ;
	}
	else
		continue_100 = "";
	requestBody = request.body;
	//std::cout << "Request Body:\n " << requestBody << std::endl;
	contentDisposition = request.getHeader("Content-Disposition");
	boundary = getBoundary(request.getHeader("Content-Type"));
	formData = parseFormData(request.body, boundary);
}

Response::~Response() {
}

void Response::distributer(std::vector<ServerConfig> &servers, std::string uri) {
	std::vector<ServerConfig>::iterator server = findServer(servers);
	location = findLocation(uri, server);
	if (errorCheck(location) == 1 || errorCheck(location) == 2) {
		if (errorCheck(location) == 1)
			logger("Request error", "error");
		NULL;
	}
	else if (method == GET) {
		findFile(uri, server);
	} else if (method == POST) {
		handlePost(uri, server);
		isValidMethod(uri);
	} else if (method == DELETE) {
		handleDelete(uri, server);
	} else {
		status_code = 405;
		reason_phrase = "Method Not Allowed";
	}
	createResponse();
}

void Response::directoryListing(void) {
	logger("Creating Directory Listing of " + path , "info");
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
				logger ("Server: [" + *serverName + "] found on port: " + toString(server->port), "info");
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
	logger ("Default server found on port: " + toString(returnServer->port), "info");
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
	if (root != "" && location.root != "" && root != location.root) {
		root = location.root;
	} else if (root == "" && location.root != "") {
		root = location.root;
	}
	logger("Root: " + root, "info");
	findFileUtil(uri, location);
	if (location.statusRedir.redirection != "" && status_code == 404 && isValidMethod(uri) == 0) {
		logger("Redirecting to: " + location.statusRedir.redirection, "info");
		status_code = location.statusRedir.status_code;
		reason_phrase = "Moved Permanently";
		http_version = "HTTP/1.1";			
		header.push_back("Location: " + location.statusRedir.redirection);
		return ;
	}
}

void Response::createBody(void) {
	//std::cout << "Creating Body " << status_code << std::endl;
	if ((status_code == 200 || status_code == 201 || status_code == 204) && skipBody == 0) {
		logger("Creating Page/File: " + path, "info");
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
			logger("Unable to open boddy file: " + path , "error");
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
	logger("Creating Error Page: " + errorPage, "info");
	if (file.is_open()) {
		while (getline(file, line)) {
			body.push_back(line);
		}
		file.close();
	}
	else {
		logger("Unable to open error page file: " + errorPage, "error");
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
	logger("RESPONSE: " + status_line, "response");
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
			logger("Error sending response", "error");
			return ;
		}
		len += sent; 
	}
	logger("Response sent", "info");
	//std::cout << "response_string: " << response_string << std::endl;
	
}
