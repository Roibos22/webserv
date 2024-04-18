/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:07 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/09 12:53:39 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include <cstdlib>

HTTPRequest::HTTPRequest(int _port) {
	port = _port;
	badRequest = 0;
}

void HTTPRequest::parseRequest(std::string request) {
	std::string line;
	std::istringstream requestStream(request);
	std::getline(requestStream, line);
	std::istringstream lineStream(line);
	std::string method;
	std::string uri;
	std::string version;
	lineStream >> method >> uri >> version;
	if (method == "GET") {
		this->method = GET;
	} else if (method == "POST") {
		this->method = POST;
	} else if (method == "DELETE") {
		this->method = DELETE;
	} else if (method == "PUT") {
		this->method = PUT;
	} else if (method == "HEAD") {
		this->method = HEAD;
	} else if (method == "OPTIONS") {
		this->method = OPTIONS;
	} else if (method == "TRACE") {
		this->method = TRACE;
	} else if (method == "CONNECT") {
		this->method = CONNECT;
	} else if (method == "PATCH") {
		this->method = PATCH;
	}
	else {
		this->method = INVALID;
		badRequest = 1;
		return ;
	}
	if (uri.empty() || version.empty()) {
		badRequest = 1;
		return ;
	}
	this->uri = uri;
	this->version = version;
	while (std::getline(requestStream, line)) {
		if (line == "\r") {
			break;
		}
		std::string key;
		std::string value;
		std::istringstream lineStream(line);
		std::getline(lineStream, key, ':');
		std::getline(lineStream, value);
		if (line.find(":") != std::string::npos && (key.empty() || value.empty())){
			badRequest = 1;
			//throw std::invalid_argument("bad request");
			return ;
		}
		this->headers[key] = value;
	}
		std::string contentLength = this->getHeader("Content-Length");
		if (contentLength != "") {
			int length = atoi(contentLength.c_str());
			char* body = new char[length];
			requestStream.read(body, length);
			this->body = std::string(body, length);
			delete[] body;
			if (this->body.size() != static_cast<std::string::size_type>(length)) {
				badRequest = 1;
				//throw std::invalid_argument("bad request");
				return ;
			}
		}
}

void HTTPRequest::parseURI(void) {
	if (uri.find("#") != std::string::npos) {
		uri = uri.substr(0, uri.find("#"));
	}
	if (uri.find("?") != std::string::npos) {
		std::string queryStr = uri.substr(uri.find("?") + 1);
		queryString = queryStr;
		uri = uri.substr(0, uri.find("?"));
		std::istringstream queryStringStream(queryStr);
		std::string pair;
		while (std::getline(queryStringStream, pair, '&')) {
			std::string key;
			std::string value;
			std::istringstream pairStream(pair);
			std::getline(pairStream, key, '=');
			std::getline(pairStream, value);
			this->query[key] = value;
		}
	}
}

std::string HTTPRequest::getHeader(std::string key) {
	std::map<std::string, std::string>::iterator it = this->headers.find(key);
	if (it != this->headers.end()) {
		return it->second;
	}
	return "";
}

void HTTPRequest::printRequest() {
	//if (this->method == 1)
	//{
		std::cout << "Method: " << this->method << std::endl;
		std::cout << "URI: " << this->uri << std::endl;
		std::cout << "Version: " << this->version << std::endl;
		std::cout << "Headers:" << std::endl;
		for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++) {
			std::cout << it->first << ": " << it->second << std::endl;
		}
	}
	 /* std::cout << "Body: " << this->body << std::endl;
	 std::cout << "Query:" << std::endl; */
/* 	 for (std::map<std::string, std::string>::iterator it = this->query.begin(); it != this->query.end(); it++) {
	 	std::cout << it->first << ": " << it->second << std::endl; */
	 


