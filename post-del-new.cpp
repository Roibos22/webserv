/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post-del-new.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 16:45:48 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/11 20:47:23 by ashojach         ###   ########.fr       */
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

// Main function to parse multipart/form-data content
FormData Response::parseFormData(const std::string& requestBody, const std::string& boundary) {
	FormData formData;

	// Boundary strings
	std::string delimiter = "--" + boundary;
	std::string endDelimiter = "--" + boundary + "--";
	
	// Find start of the first part
	size_t startPos = requestBody.find(delimiter);
	if (startPos == std::string::npos)
	{
		//std::cout << "No boundary found" << std::endl;
		return (formData);
	}
	startPos += delimiter.length();
	
	// Find the header section of the part
	size_t headerEndPos = requestBody.find("\r\n\r\n", startPos);
	if (headerEndPos == std::string::npos) {
		std::cout << "No headers found" << std::endl;
		return formData; // No headers end found
	}

	std::string headers = requestBody.substr(startPos, headerEndPos - startPos);
	std::string remaining = requestBody.substr(headerEndPos);

	// Find each header line
	size_t pos = 0;
	while ((pos = headers.find("\r\n")) != std::string::npos) {
		std::string headerLine = headers.substr(0, pos);
		headers.erase(0, pos + 2); // +2 to skip the CR LF characters

		if (headerLine.find("Content-Disposition:") != std::string::npos) {
			parseContentDisposition(headerLine, formData);
		} else if (headerLine.find("Content-Type:") != std::string::npos) {
			size_t typePos = headerLine.find(": ");
			if (typePos != std::string::npos) {
				typePos += 2; // Skip past ': '
				formData.contentType = trim(headerLine.substr(typePos));
			}
		}
	}

	// Find the start of the content
	startPos = headerEndPos + 4; // +4 to skip the CR LF characters after headers

	// Find the end of the content
	size_t contentEndPos = requestBody.find("\r\n" + endDelimiter, startPos - 4);
	if (contentEndPos == std::string::npos) {
		// If the end delimiter is not found, the content goes to the end of the requestBody
		contentEndPos = requestBody.length();
	}

	formData.content = requestBody.substr(startPos, contentEndPos - endDelimiter.length() - startPos - 4);
	formData.content = trim(formData.content); // Trim the content

	return formData;
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
	//std::cout << filepath << std::endl;
	std::ofstream file(uploadDir.c_str());

	if (file.is_open()) {
		file << content;
		file.close();
		//std::cout << "File written successfully" << std::endl;
	} else {
		//std::cerr << "Unable to open file for writing" << std::endl;
		status_code = 500;
		reason_phrase = "Internal Server Error";
		return ;
	}
	// QUESTION: Just sending back the same file again is fine?
	//path = "./test_websites/website/upload/upload.html";
	if (refer != "" && refer.find("upload") != std::string::npos)
		path = "./test_websites/website/upload/upload.html";
	else
		path = "./test_websites/default_error_pages/upload.html";
	status_code = 201;
	reason_phrase = "Created";
	//header.push_back("Content-Type: " + content_type);
	header.push_back("Content-Type: text/html");
	//header.push_back("Connection: keep-alive");
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


void Response::handleQueries( void )
{
	//std::cout << "Handling Query POST" << std::endl << queryStr << std::endl;
	// sum queries
	body.push_back("<h3>Result: " + sumVars(queryStr) + "</h3>");
	//body.push_back("<text>" +  + "</text>");

	content_type = "text/html";
	path = queryStr;
	skipBody = 1;
	status_code = 200;
	reason_phrase = "OK";
	header.push_back("Content-Type: text/html");
	header.push_back("Connection: keep-alive");
	header.push_back("Server: webserv");
}

void Response::handleUndefinedPost( void )
{
	//std::cout << "Handling Undefined POST" << std::endl;
	status_code = 200;
	reason_phrase = "OK";
	skipBody = 1;
}

void Response::handlePost(std::string uri, std::vector<ServerConfig>::iterator server)
{	
	//std::cout << "- handlePost startd" << std::endl;
	(void)server;
	//std::cout << "QUERY STRING:" << this->queryStr << std::endl;
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
	if (content_type.find("multipart/form-data") != std::string::npos) {
		handleUploadFile();
	}
	else if (!this->queryStr.empty())
		handleQueries();
	else
		handleUndefinedPost();
}

bool fileExists(const std::string& relativePath) {
	std::ifstream file(relativePath.c_str());
	bool exists = file.good();
	file.close();
	return exists;
}

void Response::deleteFileFromTmpFolder(std::string filename)
{
	//std::cout << filename << std::endl;
	std::string fullPath = "." + filename;

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
			std::cout << "File '" << filename << "' deleted successfully." << std::endl;
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

void Response::handleDelete(std::string uri, std::vector<ServerConfig>::iterator server)
{	
	(void)server;
	(void)uri;
	std::string fullPath = "." + root + uri;
	if (uri.find("/test_websites/website/upload/tmp/") != std::string::npos)
	{
		deleteFileFromTmpFolder(uri);
		return ;
	}
	if (!isFile(fullPath)) {
		status_code = 404;
		reason_phrase = "Not Found";
		//header.push_back("Content-Length: 0");
		return ;
	}
	if (remove(fullPath.c_str()) == 0) {
		status_code = 204;
		reason_phrase = "No Content";
		skipBody = 1;
	} else {
		status_code = 500;
		reason_phrase = "Internal Server Error";
	}
}

