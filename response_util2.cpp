/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_util2.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 16:31:58 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/12 00:05:27 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"
#include "webserv.hpp"

void errorMapInit(std::vector<ServerConfig> &server) {
	for (size_t i = 0; i < server.size(); i++) {
		if (server[i].errorMaps[301] == "301.html") {
			server[i].errorMaps[301] = "/test_websites/default_error_pages/301.html";
			server[i].errorKeys.push_back(301);
		}
		if (server[i].errorMaps[400] == "400.html") {
			server[i].errorMaps[400] = "/test_websites/default_error_pages/400.html";
			server[i].errorKeys.push_back(400);
		}
		if (server[i].errorMaps[403] == "403.html" || server[i].errorMaps[403] == "") {
			server[i].errorMaps[403] = "/test_websites/default_error_pages/403.html";
			server[i].errorKeys.push_back(403);
		}
		if (server[i].errorMaps[404] == "404.html") {
			server[i].errorMaps[404] = "/test_websites/default_error_pages/404.html";
			server[i].errorKeys.push_back(404);
		}
		if (server[i].errorMaps[405] == "405.html") {
			server[i].errorMaps[405] = "/test_websites/default_error_pages/405.html";
			server[i].errorKeys.push_back(405);
		}
		if (server[i].errorMaps[406] == "406.html") {
			server[i].errorMaps[406] = "/test_websites/default_error_pages/406.html";
			server[i].errorKeys.push_back(406);
		}
		if (server[i].errorMaps[408] == "408.html") {
			server[i].errorMaps[408] = "/test_websites/default_error_pages/408.html";
			server[i].errorKeys.push_back(408);
		}
		if (server[i].errorMaps[413] == "413.html") {
			server[i].errorMaps[413] = "/test_websites/default_error_pages/413.html";
			server[i].errorKeys.push_back(413);
		}
		if (server[i].errorMaps[500] == "500.html") {
			server[i].errorMaps[500] = "/test_websites/default_error_pages/500.html";
			server[i].errorKeys.push_back(500);
		}
		if (server[i].errorMaps[501] == "501.html") {
			server[i].errorMaps[501] = "/test_websites/default_error_pages/501.html";
			server[i].errorKeys.push_back(501);
		}
		if (server[i].errorMaps[505] == "505.html") {
			server[i].errorMaps[505] = "/test_websites/default_error_pages/505.html";
			server[i].errorKeys.push_back(505);
		}
	}
}

void	Response::executeScript(char **scriptArgs, char *interpreterPath)
{
	int		fd[2];
	pid_t	pid;

	if (pipe(fd) == -1) 
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	// execve script and write output into body?
	if (pid == 0)
	// Child process
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		execve(interpreterPath, scriptArgs, NULL);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else
	// Parent process
	{
		close(fd[1]);
		// TODO: somehow get rid of buffer size, it fucks things up
		const int bufferSize = 1280;
		char buffer[bufferSize];
		std::string result;
		ssize_t bytesRead;
		while ((bytesRead = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';  // Null-terminate the string
			//std::cout << buffer << std::endl;
			body.push_back(std::string(buffer));
		}
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
}

void	Response::runCgi_internal(std::string uri)
{
	char	interpreterPath[] = "/usr/bin/python3";

	std::string	cgiPath = "./test_websites/website/cgi-bin";

	std::string	fullPath = cgiPath + uri;
	char	*pathThoScript = const_cast<char*>(fullPath.c_str());
	std::cout << "**" << pathThoScript << std::endl;
	if (strcmp(pathThoScript, "./test_websites/website/cgi-bin/list_files.py") == 0)
	{
		std::cout << "list_files executing" << std::endl;
		char *scriptArgs[] = {interpreterPath, pathThoScript, NULL};
		executeScript(scriptArgs, interpreterPath);
		path = "./test_websites/website/upload/upload.html";
	}
	skipBody = 1;
	status_code = 200;
	reason_phrase = "OK";
	content_type = contentTypeFinder(path);
	header.push_back("Content-Type: " + content_type);
	header.push_back("Content-Type: text/html");
	header.push_back("Connection: keep-alive");
	header.push_back("Server: webserv");
	return;
}
