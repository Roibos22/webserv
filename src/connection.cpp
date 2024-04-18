/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/04/15 13:41:38 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "connection.hpp"

Connection::Connection(int port_, int back_log_): port(port_), backlog(back_log_) {
	memset(&this->server_addr, 0, sizeof(server_addr));
	memset(&this->fds, 0, sizeof(fds));
	//memset(&this->usedPorts, 0, sizeof(usedPorts));
}

void Connection::socket_setup(void) {
	this->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->listen_fd == -1) {
		perror("socket error");
		throw std::runtime_error("");
	}

	int opt = 1;
	if (setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		perror("setsockopt error");
		throw std::runtime_error("");
	}
	memset(&this->server_addr, 0, sizeof(server_addr));
	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_port = htons(port);
	this->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->listen_fd, (struct sockaddr*)&this->server_addr, sizeof(this->server_addr)) == -1) {
		perror("bind error");
		throw std::runtime_error("");
	}
	listen(this->listen_fd, this->backlog);
}

void Connection::poll_setup(void)
{
	this->fds[0].fd = this->listen_fd;
	this->fds[0].events = POLLIN;
	this->fds[0].revents = 0;
}