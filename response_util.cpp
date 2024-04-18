/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_util.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lgrimmei <lgrimmei@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:27:22 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/12 21:34:17 by lgrimmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "webserv.hpp"

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
	bool methodAllowed = false;
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
	for (std::vector<std::string>::iterator method_it = location.methods.begin(); method_it != location.methods.end();  ++method_it)
	{
		std::cout << methodToString(method) << "vs" << *method_it << std::endl;
		if (*method_it == methodToString(method)) {
			//std::cout << "method allowed: " << *method_it << std::endl;
			methodAllowed = true;
			break;
		}
	}
	if (!methodAllowed) {
		status_code = 405;
		reason_phrase = "Method Not Allowed";
		//header.push_back("Content-Length: 0");
		return 1;
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
			return *index;
		}
	}
	for (std::vector<std::string>::iterator index = serverIndex.begin(); index != serverIndex.end(); ++index) {
		if (isFile("." + root + uri + "/" + *index)) {
			return *index;
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
	if (root != "" && location.root != "" && root != location.root) {
		root = location.root;
	} else if (root == "" && location.root != "") {
		root = location.root;
	}
	std::string index = indexFinder(uri, location);
	if (isFile("." + root + "/" + uri)) {
		if (location.cgiParam["CGI_EXTENSION"] != "" && isEndingWithExtension(uri, trimSmeicolon(location.cgiParam["CGI_EXTENSION"])) == 1) {
			path = "." + root + uri;
			skipBody = 1;
			runCgi(uri);
			return ;
		}
		else {
			path = "." + root + uri;
		}
	} else if (isFile("." + root + uri + "/" + index)) {
		path = "." + root + uri + "/" + index;
	} else if (!isFile("." + root + uri + "/" + index) && isDirectory("." + root + "/" + uri) && (location.autoindex == true || serverAutoindex == true)) {
		skipBody = 1;
		path = "." + root + uri;
		directoryListing();
	} else if (!isFile("." + root + uri + "/" + index) && isDirectory("." + root + "/" + uri) && location.autoindex == false && serverAutoindex == false) {
		status_code = 403;
		reason_phrase = "Forbidden";
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
