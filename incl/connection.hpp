/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ashojach <ashojach@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 01:26:42 by ashojach          #+#    #+#             */
/*   Updated: 2024/04/11 16:12:01 by ashojach         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "webserv.hpp"

class Connection {
	public:
		int listen_fd;
		int port;
		int backlog;
		bool timeout;
		sockaddr_in server_addr;
		struct pollfd fds[MAX_CLIENTS];

		Connection(int port_, int bach_log_);
		void socket_setup();
		void poll_setup();
};
#endif