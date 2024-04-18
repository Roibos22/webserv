/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post-del-new.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 16:45:48 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/15 17:48:38 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "webserv.hpp"

// Helper function to trim whitespaces from the start and end of a string
std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of("\r\n\t ");
	if (std::string::npos == first) {
		return "";
	}
	size_t last = str.find_last_not_of("\r\n\t ");
	return str.substr(first, (last - first + 1));
}

// Function to parse the Content-Disposition line and extract name and filename
void parseContentDisposition(const std::string& line, FormData& formData) {
	// Find 'name' attribute
	size_t namePos = line.find("name=\"");
	if (namePos != std::string::npos) {
		namePos += 6; // Skip past 'name="'
		size_t nameEndPos = line.find("\"", namePos);
		if (nameEndPos != std::string::npos) {
			formData.name = line.substr(namePos, nameEndPos - namePos);
		}
	}

	// Find 'filename' attribute
	size_t filenamePos = line.find("filename=\"");
	if (filenamePos != std::string::npos) {
		filenamePos += 10; // Skip past 'filename="'
		size_t filenameEndPos = line.find("\"", filenamePos);
		if (filenameEndPos != std::string::npos) {
			formData.filename = line.substr(filenamePos, filenameEndPos - filenamePos);
		}
	}
}

FormData Response::parseFormData(const std::string& requestBody, const std::string& boundary) {
    FormData formData;

    std::string delimiter = "--" + boundary;
    std::string endDelimiter = delimiter + "--";

    size_t startPos = 0;
    size_t endPos = 0;

    while ((startPos = requestBody.find(delimiter, startPos)) != std::string::npos) {
        startPos += delimiter.length();
        endPos = requestBody.find(delimiter, startPos);
        if (endPos == std::string::npos) {
            endPos = requestBody.length();
        }

        std::string part = requestBody.substr(startPos, endPos - startPos);

        parsePartData(part, formData);
        startPos = endPos;
    }

    return formData;
}

void parsePartData(const std::string& part, FormData& formData) {
    size_t headerEndPos = part.find("\r\n\r\n");
    if (headerEndPos == std::string::npos) {
        return;
    }

    std::string headers = part.substr(0, headerEndPos);
    std::string content = part.substr(headerEndPos + 4);

    parseHeaders(headers, formData);
    formData.content = trim(content);
}

void parseHeaders(const std::string& headers, FormData& formData) {
    std::istringstream iss(headers);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.find("Content-Disposition:") != std::string::npos) {
            parseContentDisposition(line, formData);
        } else if (line.find("Content-Type:") != std::string::npos) {
            size_t typePos = line.find(": ");
            if (typePos != std::string::npos) {
                typePos += 2; // Skip past ': '
                formData.contentType = trim(line.substr(typePos));
            }
        }
    }
}

std::string Response::getBoundary(const std::string& contentType) {
	std::string boundaryPrefix = "boundary=";
	size_t boundaryPos = contentType.find(boundaryPrefix);

	if (boundaryPos == std::string::npos) {
		return "";
	}
	
	boundaryPos += boundaryPrefix.length();
	std::string boundary = contentType.substr(boundaryPos);
	return boundary;
}

std::string extractFilename(const std::string& input) {
	size_t pos = input.find('=');
	
	if (pos != std::string::npos) {
		return input.substr(pos + 1);
	} else {
		return "";
	}
}

void Response::handleUploadFile( void )
{	
	std::string	filename = formData.filename;
	std::string content = formData.content;
	std::string uploadDir = "." + root + location.path + "/" + filename;
	std::string directory = "." + root + location.path + "/";
	if (directory == "./test_websites/website/upload/")
		uploadDir = "." + root + location.path + "/" + "tmp/" + filename;
	std::string filepath = directory + filename;
	//std::cout << "file_path: " << filepath << std::endl;
	std::ofstream file(uploadDir.c_str(), std::ios::binary);

	if (file.is_open()) {
		//file << content;
		file.write(content.c_str(), content.length());
		file.close();
		//std::cout << "File written successfully" << std::endl;
		logger("File written successfully", "info");
	} else {
		//std::cerr << "Unable to open file for writing" << std::endl;
		logger("Unable to open file for writing", "error");
		status_code = 500;
		reason_phrase = "Internal Server Error";
		return ;
	}
	path = "./test_websites/website/upload/upload.html";
	if (refer != "" && refer.find("upload") != std::string::npos)
		path = "./test_websites/website/upload/upload.html";
	else
		path = "./test_websites/default_error_pages/upload.html";
	status_code = 201;
	reason_phrase = "Created";
	header.push_back("Content-Type: text/html");
	header.push_back("Connection: closed");
	header.push_back("Server: webserv");

}

bool isNumber(const std::string& s) {
	if (s.empty()) return false;
	for (size_t i = 0; i < s.length(); i++) {
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}

std::string sumVars(const std::string& queryString)
{
	std::istringstream iss(queryString);
	std::map<std::string, int> vars;
	std::string pair, key, value;
	int sum = 0;

	// Parse the query string
	while (getline(iss, pair, '&')) {
		std::istringstream kvp(pair);
		getline(kvp, key, '=');
		getline(kvp, value);
		if (!isNumber(value))
			return "Error: Non-numeric value provided for " + key;
		vars[key] = atoi(value.c_str());
	}

	// Sum up all the values
	for (std::map<std::string, int>::iterator it = vars.begin(); it != vars.end(); ++it) {
		sum += it->second;
	}

	// Convert sum to string
	std::ostringstream oss;
	oss << sum;
	return oss.str();
}


void Response::handleQueries( void ) {
	content_type = "text/html";
	status_code = 200;
	reason_phrase = "OK";
	header.push_back("Content-Type: text/html");
	header.push_back("Connection: closed");
	header.push_back("Server: webserv");
	body.push_back("<html><body>");
	body.push_back("<p>Query String: " + queryStr + " Received" + "</p>");
	while (queryStr.find("&") != std::string::npos) {
		size_t pos = queryStr.find("&");
		body.push_back("<p>" + queryStr.substr(0, pos) + "</p>");
		queryStr = queryStr.substr(pos + 1);
	}
	body.push_back("<p>" + queryStr + "</p>");
	body.push_back("</body></html>");
	
}

void Response::handleUndefinedPost( void )
{
	//std::cout << "Handling Undefined POST" << std::endl;
	status_code = 200;
	reason_phrase = "OK";
	header.push_back("Connection: closed");
	header.push_back("Server: webserv");
	skipBody = 1;
}

void Response::handlePost(std::string uri, std::vector<ServerConfig>::iterator server) {	
	(void)server;
	if (root != "" && location.root != "" && root != location.root) {
		root = location.root;
	} else if (root == "" && location.root != "") {
		root = location.root;
	}
	
	if (isFile("." + root + uri)) {
		if (location.cgiParam["CGI_EXTENSION"] != "" && isEndingWithExtension(uri, trimSmeicolon(location.cgiParam["CGI_EXTENSION"])) == 1) {
			path = "." + root + uri;
			skipBody = 1;
			runCgi(uri);
			return ;
		}
		else {
			status_code = 405;
			reason_phrase = "Method Not Allowed";
			return ;
		}
	}
	if (isDirectory("." + root + uri) && content_type.find("multipart/form-data") != std::string::npos && !isValidMethod(uri)) {
		handleUploadFile();
	}
	else if (isDirectory("." + root + uri) && !this->queryStr.empty()) {
		handleQueries();
	}
	else if (isDirectory("." + root + uri)) {
		handleUndefinedPost();
	} else {
		status_code = 404;
		reason_phrase = "Not Found";
	}
}

bool fileExists(const std::string& relativePath) {
	std::ifstream file(relativePath.c_str());
	bool exists = file.good();
	file.close();
	return exists;
}

void Response::deleteFileFromTmpFolder(std::string filename) {
	std::string fullPath = "." + filename;
	logger("Deleting file '" + filename + "'" + " from: " + full_path , "info");
	if (filename.find("/test_websites/website/upload/tmp/") != 0)
	{
		status_code = 401;
		reason_phrase = "Unauthorized";
		//path = "./test_websites/website/pages/401.html";
	}
	else if (!fileExists(fullPath))
	{
		status_code = 404;
		reason_phrase = "File Not Found";
		//path = "./test_websites/website/pages/404.html";
	}
	else
	{
		if (remove(fullPath.c_str()) == 0) { 
			//std::cout << "File '" << filename << "' deleted successfully." << std::endl;
			logger("File '" + filename + "' deleted successfully.", "info");
			status_code = 204;
			reason_phrase = "No Content";
			skipBody = 1;
		} else {
			status_code = 500;
			reason_phrase = "Internal Server Error";
		}
	}
	header.push_back("Server: webserv");
}

void Response::handleDelete(std::string uri, std::vector<ServerConfig>::iterator server) {	
	(void)server;
	(void)uri;

	std::string fullPath = "." + root + uri;
	if (!isFile(fullPath)) {
		status_code = 404;
		reason_phrase = "Not Found";
		return ;
	}
	for (std::vector<std::string>::iterator method_it = location.methods.begin(); method_it != location.methods.end(); ++method_it) {
		if (*method_it == methodToString(method)) {
			break ;
		}
		else if (method_it == location.methods.end() - 1){
			status_code = 405;
			reason_phrase = "Method Not Allowed";
			return ;
		}
	}
	
	if (uri.find("/test_websites/website/upload/tmp/") != std::string::npos)
	{
		deleteFileFromTmpFolder(uri);
		return ;
	}
	if (remove(fullPath.c_str()) == 0) {
		status_code = 204;
		reason_phrase = "No Content";
		header.push_back("Server: webserv");
		header.push_back("Connection: closed");
		skipBody = 1;
	} else {
		status_code = 500;
		reason_phrase = "Internal Server Error";
	}
}

