/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 22:36:36 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/13 15:51:06 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "request.hpp"
#include <sstream>
#include <string>

void Response::defineEnv(std::map<std::string, std::string>	&envMap) {
	// std::cout << "CGI PATH: " << location.cgiParam["PATH_INFO"] << std::endl;
	// std::cout << "CGI EXTENSION: " << location.cgiParam["CGI_EXTENSION"] << std::endl;
	if ( location.cgiParam["GATEWAY_INTERFACE"] != "") {
		envMap["GATEWAY_INTERFACE"] = trimSmeicolon( location.cgiParam["GATEWAY_INTERFACE"]);
	} else {
		envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	}
	if ( location.cgiParam["SERVER_NAME"] != "") {
		envMap["SERVER_NAME"] = trimSmeicolon( location.cgiParam["SERVER_NAME"]);
	} else {
		envMap["SERVER_NAME"] = host;
	}
	if ( location.cgiParam["SERVER_PORT"] != "") {
		envMap["SERVER_PORT"] = trimSmeicolon( location.cgiParam["SERVER_PORT"]);
	} else {
		envMap["SERVER_PORT"] = toString(port);
	}
	if ( location.cgiParam["SERVER_PROTOCOL"] != "") {
		envMap["SERVER_PROTOCOL"] = trimSmeicolon( location.cgiParam["SERVER_PROTOCOL"]);
	} else {
		envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	}
	if ( location.cgiParam["SERVER_SOFTWARE"] != "") {
		envMap["SERVER_SOFTWARE"] = trimSmeicolon( location.cgiParam["SERVER_SOFTWARE"]);
	} else {
		envMap["SERVER_SOFTWARE"] = "webserv";
	}
	if ( location.cgiParam["REDIRECT_STATUS"] != "") {
		envMap["REDIRECT_STATUS"] = trimSmeicolon( location.cgiParam["REDIRECT_STATUS"]);
	} else {
		envMap["REDIRECT_STATUS"] = "200";
	}
	if ( location.cgiParam["SCRIPT_NAME"] != "") {
		envMap["SCRIPT_NAME"] = trimSmeicolon( location.cgiParam["SCRIPT_NAME"]);
	} else {
		envMap["SCRIPT_NAME"] = path;
	}
	if ( location.cgiParam["REQUEST_URI"] != "") {
		envMap["REQUEST_URI"] = trimSmeicolon( location.cgiParam["REQUEST_URI"]);
	} else {
		envMap["REQUEST_URI"] = path;
	}
	if ( location.cgiParam["AUTH_TYPE"] != "") {
		envMap["AUTH_TYPE"] = trimSmeicolon( location.cgiParam["AUTH_TYPE"]);
	} else {
		envMap["AUTH_TYPE"] = "";
	}
	if ( location.cgiParam["CONTENT_LENGTH"] != "") {
		envMap["CONTENT_LENGTH"] = trimSmeicolon( location.cgiParam["CONTENT_LENGTH"]);
	} else {
		envMap["CONTENT_LENGTH"] = toString(requestBodySize);
	}
	if ( location.cgiParam["CONTENT_TYPE"] != "") {
		envMap["CONTENT_TYPE"] = trimSmeicolon( location.cgiParam["CONTENT_TYPE"]);
	} else {
		envMap["CONTENT_TYPE"] = "text/html";
	}
	if ( location.cgiParam["PATH_INFO"] != "") {
		envMap["PATH_INFO"] = trimSmeicolon( location.cgiParam["PATH_INFO"]);
	} else {
		envMap["PATH_INFO"] = path;
	}
	if ( location.cgiParam["PATH_TRANSLATED"] != "") {
		envMap["PATH_TRANSLATED"] = trimSmeicolon( location.cgiParam["PATH_TRANSLATED"]);
	} else {
		envMap["PATH_TRANSLATED"] = path;
	}
	if ( location.cgiParam["QUERY_STRING"] != "") {
		envMap["QUERY_STRING"] = trimSmeicolon( location.cgiParam["QUERY_STRING"]);
	} else {
		envMap["QUERY_STRING"] = queryStr;
	}
	if ( location.cgiParam["REMOTE_ADDR"] != "") {
		envMap["REMOTE_ADDR"] = trimSmeicolon( location.cgiParam["REMOTE_ADDR"]);
	} else {
		envMap["REMOTE_ADDR"] = "";
	}
	if ( location.cgiParam["REMOTE_HOST"] != "") {
		envMap["REMOTE_HOST"] = trimSmeicolon( location.cgiParam["REMOTE_HOST"]);
	} else {
		envMap["REMOTE_HOST"] = "";
	}
	if ( location.cgiParam["REMOTE_IDENT"] != "") {
		envMap["REMOTE_IDENT"] = trimSmeicolon( location.cgiParam["REMOTE_IDENT"]);
	} else {
		envMap["REMOTE_IDENT"] = "";
	}
	if ( location.cgiParam["REMOTE_USER"] != "") {
		envMap["REMOTE_USER"] = trimSmeicolon( location.cgiParam["REMOTE_USER"]);
	} else {
		envMap["REMOTE_USER"] = "";
	}
	if ( location.cgiParam["REQUEST_METHOD"] != "") {
		envMap["REQUEST_METHOD"] = trimSmeicolon( location.cgiParam["REQUEST_METHOD"]);
	} else {
		envMap["REQUEST_METHOD"] = methodToString(method);
	}
	if ( location.cgiParam["SCRIPT_FILENAME"] != "") {
		envMap["SCRIPT_FILENAME"] = trimSmeicolon( location.cgiParam["SCRIPT_FILENAME"]);
	} else {
		envMap["SCRIPT_FILENAME"] = path;
	}
	if ( location.cgiParam["SERVER_ADMIN"] != "") {
		envMap["SERVER_ADMIN"] = trimSmeicolon( location.cgiParam["SERVER_ADMIN"]);
	} else {
		envMap["SERVER_ADMIN"] = "";
	}
	if ( location.cgiParam["SERVER_PORT_SECURE"] != "") {
		envMap["SERVER_PORT_SECURE"] = trimSmeicolon( location.cgiParam["SERVER_PORT_SECURE"]);
	} else {
		envMap["SERVER_PORT_SECURE"] = "";
	}
	envMap["REQUEST_BODY"] = requestBody;
}

void Response::runCgi(std::string uri) {
	(void)uri;
	std::map<std::string, std::string>	envMap;
	defineEnv(envMap);
	std::vector<std::string> envVars;
	for (std::map<std::string, std::string>::iterator it = envMap.begin(); it != envMap.end(); ++it) {
		envVars.push_back(it->first + "=" + it->second);
	}
	char** env = new char*[envVars.size() + 1];
	for (size_t i = 0; i < envVars.size(); ++i) {
		env[i] = const_cast<char*>(envVars[i].c_str());
	}
	//std::cout << "queryStr: " << queryStr << std::endl;
	logger("Running CGI with Query string: " + queryStr + " and/or Request Body: " + requestBody, "info");
	env[envVars.size()] = NULL;
	const char* args[] = {envMap["PATH_INFO"].c_str(), path.c_str(), NULL}; 
	int pipefd[2];
	int inputPipefd[2];
	if (pipe(pipefd) == -1 || pipe(inputPipefd) == -1) {
		perror("pipe");
	}
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
	}
	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		//close(pipefd[1]);

		close(inputPipefd[1]);
		dup2(inputPipefd[0], 0);
		close(inputPipefd[0]);
		
		execve(envMap["PATH_INFO"].c_str(), (char* const*)args, env);
		perror("execve");
	}
	else {
		//close(pipefd[1]);
		close(inputPipefd[0]);
		write(inputPipefd[1], requestBody.c_str(), requestBody.size());
		close(inputPipefd[1]);
		int status;
		//waitpid(pid, &status, 0);
		sleep(1);
		if (waitpid(pid, &status, WNOHANG) == 0) {
			kill(pid, SIGKILL);
		}
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0){
			//std::cout << "child exited with status " << WEXITSTATUS(status) << std::endl;
			logger("CGI ran successfully", "info");
		} else {
			//std::cout << "child failed" << std::endl;
			logger("CGI failed", "error");
			http_version = "HTTP/1.1";
			status_code = 500;
			reason_phrase = "Internal Server Error";
			content_type = "text/html";
			header.push_back("Content-Type: " + content_type);
			header.push_back("Connection: close");
			header.push_back("Server: webserv");
			skipBody = 0;
			delete[] env;
			return ;
		}
		close(pipefd[1]);
		char buffer[4096];
		std::string response = "";
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			response.append(buffer, bytesRead);
			//memset(buffer, 0, sizeof(buffer));
		}
		std::size_t pos = response.find("\r\n\r\n");
		if (pos != std::string::npos) {
			std::string headers = response.substr(0, pos);
			std::string bodyStr = response.substr(pos + 4);
			std::istringstream responseStream(bodyStr);
			std::string line;
			while (std::getline(responseStream, line)) {
				body.push_back(line);
				//std::cout << line << std::endl;
			}
		}
		else {
			std::cout << "no headers" << std::endl;
		}
	}
	delete[] env;
	if (uri.find("internal-cgi") != std::string::npos) {
		http_version = "HTTP/1.1";
		status_code = 200;
		reason_phrase = "OK";
		content_type = "text/html";
		header.push_back("Content-Type: " + content_type);
		header.push_back("Connection: close");
		header.push_back("Server: webserv");
		path = "./test_websites/website/upload/upload_file.html";
		return ;
	}
	http_version = "HTTP/1.1";
	status_code = 200;
	reason_phrase = "OK";
	content_type = "text/html";
	header.push_back("Content-Type: " + content_type);
	header.push_back("Connection: close");
	header.push_back("Server: webserv");
}
