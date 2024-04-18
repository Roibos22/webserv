/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_util.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:22 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/15 21:06:07 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "webserv.hpp"

extern int g_logger;

std::string contentTypeFinder(std::string path) { 
	std::string extension = path.substr(path.find_last_of(".") + 1);
	if (extension == "html")
		return("text/html");
	else if (extension == "css")
		return("text/css");
	else if (extension == "js")
		return("text/javascript");
	else if (extension == "jpg" || extension == "jpeg")
		return("image/jpeg");
	else if (extension == "png")
		return("image/png");
	else if (extension == "gif")
		return("image/gif");
	else if (extension == "bmp")
		return("image/bmp");
	else if (extension == "tiff" || extension == "tif")
		return("image/tiff");
	else if (extension == "webp")
		return("image/webp");
	else if (extension == "eot")
		return("application/vnd.ms-fontobject");
	else if (extension == "ttf")
		return("font/ttf");
	else if (extension == "otf")
		return("font/otf");
	else if (extension == "woff")
		return("font/woff");
	else if (extension == "woff2")
		return("font/woff2");
	else if (extension == "json")
		return("application/json");
	else if (extension == "xml")
		return("application/xml");
	else if (extension == "rss")
		return("application/rss+xml");
	else if (extension == "atom")
		return("application/atom+xml");
	else if (extension == "m3u8")
		return("application/x-mpegURL");
	else if (extension == "ts")
		return("video/MP2T");
	else if (extension == "flv")
		return("video/x-flv");
	else if (extension == "svg")
		return("image/svg+xml");
	else if (extension == "mp3")
		return("audio/mpeg");
	else if (extension == "mp4")
		return("video/mp4");
	else if (extension == "txt")
		return("text/plain");
	else if (extension == "ico")
		return ("image/x-icon");
	else if (extension == "pdf")
		return ("application/pdf");
	else if (extension == "zip")
		return ("application/zip");
	else if (extension == "rar")
		return ("application/x-rar-compressed");
	else
		return "";
}

int isFile(std::string uri) {
	struct stat buffer;
	if (stat(uri.c_str(), &buffer) != 0) {
		return 0;
	}
	return S_ISREG(buffer.st_mode);
}

int isDirectory(std::string uri) {
	struct stat buffer;
	if (stat(uri.c_str(), &buffer) != 0) {
		return 0;
	}
	return S_ISDIR(buffer.st_mode);
}

std::string findFileExtension(std::string uri) {
	size_t pos = uri.find_last_of(".");
	if (pos != std::string::npos) {
		return uri.substr(pos);
	}
	return "";
}

std::string methodToString(HTTPMethod method) {
	if (method == GET)
		return "GET";
	else if (method == POST)
		return "POST";
	else if (method == DELETE)
		return "DELETE";
	else
		return "";
}

int findPort(std::string host) {
	size_t pos = host.find(":");
	if (pos != std::string::npos) {
		//return std::stoi(host.substr(pos + 1));
		return std::atoi(host.substr(pos + 1).c_str());
	}
	return 80;
}

int Response::errorCheck(LocationConfig location) 
{
	(void)location;
	if (badRequest == 1)
	{
		status_code = 400;
		reason_phrase = "Bad Request";
		return 1;
	}
	if (http_version != "HTTP/1.1")
	{
		status_code = 505;
		reason_phrase = "HTTP Version Not Supported";
		return 1;
	}
	if (continue_100 == "100-continue")
	{
		status_code = 417;
		reason_phrase = "Expectation Failed";
		//header.push_back("Connection: close");
		header.push_back("Server: webserv");
		skipBody = 1;
		return 2;
	}
	if (method == POST && requestBodySize > maxBodySize)
	{
		status_code = 413;
		reason_phrase = "Payload Too Large";
		return 1;
	}
	// error 501 should be implemented
	return 0;
}

std::string whiteSpaceTrim(std::string &str) {
	size_t start = str.find_first_not_of(" \t \n \r \f \v");
	size_t end = str.find_last_not_of(" \t \n \r \f \v");
	if (start == std::string::npos || end == std::string::npos) {
		return "";
	}
	std::string str_ = str.substr(start, end - start + 1);
	return str_;
}

std::string Response::indexFinder(std::string uri, LocationConfig &location) {
	for (std::vector<std::string>::iterator index = location.index.begin(); index != location.index.end(); ++index) {
		if (isFile("." + root + uri + "/" + *index)) {
			if (uri == "/")
				return *index;
			else
				return "/" + *index;
		}
	}
	for (std::vector<std::string>::iterator index = serverIndex.begin(); index != serverIndex.end(); ++index) {
		if (isFile("." + root + uri + "/" + *index)) {
			//return *index;
			if (uri == "/")
				return *index;
			else
				return "/" + *index;
		}
	}
	return "";
}

std::string trimSmeicolon(std::string &str) {
	size_t pos = str.find(";");
	if (pos != std::string::npos) {
		str = str.substr(0, pos);
		return str;
	}
	return str;
}

int isEndingWithExtension(std::string uri, std::string extension) {
	size_t pos = uri.find_last_of(".");
	if (pos != std::string::npos) {
		if (uri.substr(pos) == extension) {
			return 1;
		}
	}
	return 0;
}

void Response::findFileUtil(std::string uri, LocationConfig &location) {
	/* if (root != "" && location.root != "" && root != location.root) {
		root = location.root;
	} else if (root == "" && location.root != "") {
		root = location.root;
	} */
	int allowedMethod = getIsValidMethod(uri);
	std::string index = indexFinder(uri, location);
	if (isFile("." + root + "/" + uri) && allowedMethod == 0) {
		if (location.cgiParam["CGI_EXTENSION"] != "" && isEndingWithExtension(uri, trimSmeicolon(location.cgiParam["CGI_EXTENSION"])) == 1) {
			path = "." + root + uri;
			skipBody = 1;
			runCgi(uri);
			return ;
		}
		else {
			path = "." + root + uri;
			logger("File found: " + path, "info");
		}
	} else if (isFile("." + root + uri + "/" + index) && allowedMethod == 0) {
		path = "." + root + uri + index;
		logger("Index File found: " + index, "info");
	} else if (!isFile("." + root + uri + "/" + index) && isDirectory("." + root + "/" + uri) && (location.autoindex == true || serverAutoindex == true) && allowedMethod == 0) {
		skipBody = 1;
		path = "." + root + uri;
		directoryListing();
	} else if (!isFile("." + root + uri + "/" + index) && isDirectory("." + root + "/" + uri) && location.autoindex == false && serverAutoindex == false && allowedMethod == 0) {
		status_code = 403;
		reason_phrase = "Forbidden";
		return ;
	} else if (allowedMethod == 1) {
		status_code = 405;
		reason_phrase = "Method Not Allowed";
		skipBody = 0;
		return ;
	} else {
		status_code = 404;
		reason_phrase = "Not Found";
		return ;
	}
	
	status_code = 200;
	reason_phrase = "OK";
	content_type = contentTypeFinder(path);
	header.push_back("Content-Type: " + content_type);
	header.push_back("Connection: close");
	header.push_back("Server: webserv");
}


int Response::getIsValidMethod(std::string uri) {
	(void)uri;
	for (std::vector<std::string>::iterator method_it = location.methods.begin(); method_it != location.methods.end(); ++method_it) {
		if (*method_it == methodToString(method)) {
			break ;
		}
		else if (method_it == location.methods.end() - 1) {
			return 1;
		}
	}
	return 0;
}
int Response::isValidMethod(std::string uri) {
	(void)uri;
	for (std::vector<std::string>::iterator method_it = location.methods.begin(); method_it != location.methods.end(); ++method_it) {
		if (*method_it == methodToString(method)) {
			break ;
		}
		else if (method_it == location.methods.end() - 1) {
			if (status_code != 404) {
				status_code = 405;
				reason_phrase = "Method Not Allowed";
				skipBody = 0;
				return 1;
			}
		}
	}
	return 0;
}

void logger(std::string log, std::string type) {
	#define RED "\033[1;31m"
	#define GREEN "\033[1;32m"
	#define YELLOW "\033[1;33m"
	#define BLUE "\033[1;34m"

	if (g_logger != 1)
		return ;
	if (type == "request")
		std::cout << BLUE << log << "\033[0m" << std::endl;
	else if (type == "response")
		std::cout << GREEN << log << "\033[0m" << std::endl;
	else if (type == "error")
		std::cout << RED << log << "\033[0m" << std::endl;
	else if (type == "info")
		std::cout << YELLOW << log << "\033[0m" << std::endl;
	else if (type == "legend")
		std::cout << log << RED << "error " << GREEN << "response " << YELLOW << "info " << BLUE << "request" << "\033[0m" << std::endl << std::endl;
}