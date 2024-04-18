/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lgrimmei <lgrimmei@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:26:56 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/18 15:54:15 by lgrimmei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "request.hpp"
#include "response.hpp"
#include "connection.hpp"
#include <signal.h>
#include <set>
#include <ctime>
#include <cstddef>
#include "configParser.hpp"
#include "Utils.hpp"

Utils ut3;

using std::cout;
using std::endl;

int g_logger = 1; // logging: 1 = on, -1 = off

void sigint_handler(int signum) {
	(void)signum;
	std::cout << "\nSIGINT received. Shutting down server..." << std::endl;
	g_logger = 0;
}

void handleCunkedRequest(HTTPRequest *request, int fd) {
	std::string chunkedData;
	while (true) {
		std::string chunkSizeStr;
		char c;
		while (read(fd, &c, 1) > 0 && c != '\n') {
			chunkSizeStr += c;
		}
		//int chunkSize = std::stoi(chunkSizeStr, nullptr, 16);
		int chunkSize = std::atoi(chunkSizeStr.c_str());
		if (chunkSize == 0)
			break;
		char chunkData[chunkSize];
		recv(fd, chunkData, chunkSize, 0);
		chunkedData += std::string(chunkData, chunkSize);
		char crlf[2];
		recv(fd, crlf, 2, 0);
	}
	request->body = chunkedData;
}

int handleTimeout(int fd, std::time_t last_time, std::set<int> *active_fds) {
	std::time_t now = std::time(0);
	std::time_t last_time_;
	if (last_time == 0)
		last_time_= now;
	else
		last_time_ = last_time;
	if (now - last_time_ > 60 && active_fds->count(fd)) { // 10 seconds timeout --> nginx default is 60 seconds
		return (1);
	}
	return (0);
}

int main(int ac, char **av, char **env)
{
	if (g_logger != 1 && g_logger != -1) {
		std::cerr << "Error! g_logger value should be either 1 or -1" << std::endl;
		return (1);
	}
	signal(SIGINT, &sigint_handler);
	std::vector<ServerConfig> servers;
	std::vector<Connection> connections;
	std::set<int> active_fds;
	std::set<int> usedPorts;
	std::map<int, std::time_t> last_data_time;
	std::string file;
	try {
		if(ac < 2)
			file = "./ConfigFiles/ar1.conf";
		else if(ac > 2)
		{
			std::cerr << "Too many arguments god damn it !\nWe only want one config file as argument..\n";
			return (0);
		}
		else
			file = av[1];
		configParser conf(file, env);
		//conf.outputParse();
		servers = ut3.convListToVector(conf.getServerList());
		errorMapInit(servers);
		for (size_t i = 0; i < servers.size(); i++) {
			if (usedPorts.count(servers[i].port)) {
				std::cout << "--> Port " << servers[i].port << " is already in use. More servers will listen on the same port <--" << std::endl;
				continue ;
			}
			Connection connection(servers[i].port, 100);
			connection.socket_setup();
			connection.poll_setup();
			connections.push_back(connection);
			usedPorts.insert(servers[i].port);
		}
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Error in config file. Server failed to start" << std::endl;
		return (1);
	}
	
	std::cout << "Server started" << std::endl;
	if (g_logger == 1)
		logger  ("Log Color Guide: ", "legend");
	
	int nfds = 1, new_fd;
	for (size_t i = 0; i < connections.size(); i++) {
		if (fcntl(connections[i].listen_fd, F_SETFL, O_NONBLOCK) == -1) {
			perror("fcntl error");
			continue ;
		}
	}
	while (1) {
		for (size_t i = 0; i < connections.size(); i++) {
			int n = poll(connections[i].fds, nfds, 10);
			if (n == -1) {
				//perror("poll");
				continue ;
			}
			for (int j = 0; j < nfds; j++) {
				if (handleTimeout(connections[i].fds[j].fd, last_data_time[connections[i].fds[j].fd], &active_fds)){
					logger("Timeout", "error");
					send (connections[i].fds[j].fd, "HTTP/1.1 408 Request Timeout\r\n\r\n", 30, 0);
					if (close(connections[i].fds[j].fd) == -1) {
						perror("#1 close error");
					}
					last_data_time.erase(connections[i].fds[j].fd);
					active_fds.erase(connections[i].fds[j].fd);
					connections[i].fds[j].fd = -1;
					for (int k = j; k < nfds; k++) {
						connections[i].fds[k] = connections[i].fds[k + 1];
					}
					nfds--;
				}
				if (connections[i].fds[j].revents == 0) {
					continue ;
				}
				if (connections[i].fds[j].fd == connections[i].listen_fd) {
					new_fd = accept(connections[i].listen_fd, NULL, NULL);
					if (new_fd == -1) {
						perror("accept error");
						continue ;
					} else {
						fcntl(new_fd, F_SETFL, O_NONBLOCK);
						connections[i].fds[nfds].fd = new_fd;
						connections[i].fds[nfds].events = POLLIN;
						active_fds.insert(new_fd); 
						nfds++;
						last_data_time[new_fd] = std::time(0);
					}
				} else {
					char buffer[1024];
					std::string buff_str = "";
					int len = 0;
					int total_len = 0;
					if (active_fds.count(connections[i].fds[j].fd)) {
						while ((len = recv(connections[i].fds[j].fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
							buff_str.append(buffer, buffer + len);
							total_len += len;
							last_data_time[connections[i].fds[j].fd] = std::time(0);
						}
					}
					if (!buff_str.empty()) {
						std::string buffer_str = buff_str;
						HTTPRequest *request = new HTTPRequest(connections[i].port);
						request->parseRequest(buffer_str);
						request->parseURI();
						logger("NEW REQUEST: " + methodToString(request->method) + " " + request->uri + " " + request->version, "request");
						if (request->headers["Transfer-Encoding"] == "chunked") {
							handleCunkedRequest(request, connections[i].fds[j].fd);
						}
						Response *response = new Response(*request);
						response->distributer(servers, request->uri);
						response->sendResponse(connections[i].fds[j].fd);
						delete response;
						delete request;
					} else if (len == 0) {
						if (close(connections[i].fds[j].fd) == -1) {
							perror("#1 close error");
						}
						last_data_time.erase(connections[i].fds[j].fd);
						active_fds.erase(connections[i].fds[j].fd);
						connections[i].fds[j].fd = -1;
						for (int k = j; k < nfds; k++) {
							connections[i].fds[k] = connections[i].fds[k + 1];
						}
						nfds--;
						logger("Connection closed", "info");
					} else if (len == -1) {
						continue ;
					}
				}
			}
		}
		if (g_logger == 0) {
			break ;
		}
	}
	std::cout << "Server stopped" << std::endl;
	return (0);
}