/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zaabou <zaabou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:32 by zaabou            #+#    #+#             */
/*   Updated: 2023/05/23 09:12:40 by zaabou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "WebServ.hpp"
#include <Request.hpp>
#include <Response.hpp>
#include <Http.hpp>
#include <Server.hpp>
#include <exception>
#include <netinet/in.h>
#include <string>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_timeval.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <Utils.hpp>
#include <sys/time.h>

#define BUFFER_SIZE 1024

// ends of receiving the data
# define SERVER_READ_END 0 
# define CGI_WRITE_END 1

// ends of sending the data
# define CGI_READ_END 2
# define SERVER_WRITE_END 3

class Cgi {
    private :
        // response component 
		std::vector<std::string>	_cookies;
        std::string					_content_type;
        size_t						_content_length;
        std::string					_body;
        short						_status;
		std::string					_location;
		

        // cgi attribute
		std::string							_cgi;
        std::vector<int>					_fds;
        std::vector<std::string>			_env;

		// default constructor
						Cgi();
		
	public:
		// parameter constructor
		explicit		Cgi(const std::string &cgi) throw(int);
		// destructor
						~Cgi();
		// setup the environment to cgi and webserver processes
		void			run(const Request& request, const Server& server, const client_info& client, const std::string& path) throw(int);
		// create pipes to exchange data
		void			create_communication_channel(const std::string &method) throw(int);
		void			parent_work(const Request& request, pid_t pid) throw(int);
		void			send_request(const Request& request) throw(int);
		void			receive_response() throw(int);
		void			parse_headers(std::string &response, bool &start_body) throw();
		void			wait_for_child(pid_t pid) throw(int);
		void			child_work(const Request& request, const Server& server, const client_info& client, const std::string& path) throw();
		void			setup_environment(const Request& request, const Server& server, const client_info& client, const std::string& path) throw();
		static bool		check_script_permission(const std::string& path) throw();

		// getters for the response attributes
		std::vector<std::string>	getCookies() const;
		std::string					getContentType() const;
		size_t						getContentLength() const;
		std::string					getBody() const;
		short						getStatus() const;
		std::string					getLocation() const;
};

#endif
