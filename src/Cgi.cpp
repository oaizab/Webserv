/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zaabou <zaabou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 15:33:20 by zaabou            #+#    #+#             */
/*   Updated: 2023/05/23 11:09:03 by zaabou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cstring>
#include <iostream>
#include "Utils.hpp"
#include "WebServ.hpp"
#include "statusCodes.hpp"
#include <Cgi.hpp>
#include <__nullptr>
#include <_ctype.h>
#include <algorithm>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <string>
#include <strings.h>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/_types/_time_t.h>
#include <sys/_types/_timeval.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>

Cgi::Cgi(const std::string &cgi)  throw(int) :  _content_length(std::string::npos), _status(200), _cgi(cgi)
{
	
	if (this->_cgi.front() == '/' and access(this->_cgi.c_str(), X_OK) == -1)
			throw INTERNAL_SERVER_ERROR;
	else if (this->_cgi.front() != '/')
	{
		this->_cgi = Utils::getAbsolutePath(this->_cgi);
		if (this->_cgi.empty())
			throw INTERNAL_SERVER_ERROR;
	}
}

Cgi::~Cgi() {}

void    Cgi::run(const Request& request, const Server& server, const client_info& client, const std::string& path) throw(int)
{
    pid_t	pid;

    if (not this->check_script_permission(path))
        throw FORBIDDEN;
	
	this->create_communication_channel(request.method());
    pid = fork();
    if (pid == -1)
        throw INTERNAL_SERVER_ERROR;
    if (pid == 0)
        this->child_work(request, server, client, path);
    else
        this->parent_work(request, pid);
	
}

void	Cgi::create_communication_channel(const std::string &method) throw(int)
{
	this->_fds.assign(4, 0);
	if (pipe(&this->_fds[0]) == -1)
		throw INTERNAL_SERVER_ERROR;
	if (method == "POST")
	{
		if (pipe(&this->_fds[2]) == -1)
			throw INTERNAL_SERVER_ERROR;
	}
}

void    Cgi::parent_work(const Request& request, pid_t pid) throw(int)
{
	try {
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
			throw INTERNAL_SERVER_ERROR;
		if (request.method() == "POST")
			this->send_request(request);
		this->receive_response();
		this->wait_for_child(pid);
	} catch (int& e) {
		kill(pid, SIGKILL);
		throw;
	}
	if (signal(SIGPIPE, SIG_DFL) == SIG_ERR)
		throw INTERNAL_SERVER_ERROR;
}

void    Cgi::send_request(const Request& request) throw(int)
{
	size_t      pos = 0;
    ssize_t     nbytes;
	time_t		start = time(nullptr);
	std::string	buffer;
	
	if (close(this->_fds.at(CGI_READ_END)) == -1)
		throw INTERNAL_SERVER_ERROR;
	if (fcntl(this->_fds.at(SERVER_WRITE_END), F_SETFL, O_NONBLOCK) == -1)
		throw INTERNAL_SERVER_ERROR;
	while (pos < request.body().to_string().length())
	{
		if (time(nullptr) - start >= 30)
			throw  GATEWAY_TIMEOUT;
		buffer = request.body().to_string().substr(pos, BUFFER_SIZE);
		nbytes = write(this->_fds.at(SERVER_WRITE_END), buffer.c_str(), buffer.length());
		if (nbytes != -1)
			pos += nbytes;
		else if (errno != EAGAIN)
			throw INTERNAL_SERVER_ERROR;
	}
	if (close(this->_fds.at(SERVER_WRITE_END)) == -1)
		throw INTERNAL_SERVER_ERROR;
}

void    Cgi::receive_response() throw(int)
{
	char		buffer[BUFFER_SIZE];
	std::string	resp;
	ssize_t		nbytes;
	time_t		start = time(nullptr);
	bool		start_body = false;

	if (close(this->_fds.at(CGI_WRITE_END)) == -1)
		throw INTERNAL_SERVER_ERROR;
	if (fcntl(this->_fds.at(SERVER_READ_END), F_SETFL, O_NONBLOCK) == -1)
		throw INTERNAL_SERVER_ERROR;
	while ((nbytes = read(this->_fds.at(SERVER_READ_END), buffer, BUFFER_SIZE)) != 0)
	{
		if (time(nullptr) - start >= 30)
			throw GATEWAY_TIMEOUT;
		if (nbytes != -1)
		{
			resp.append(buffer, nbytes);
			if (start_body == false)
				this->parse_headers(resp, start_body);
			else if (resp.length() >= this->_content_length)
				break ;
		}
		else if (errno != EAGAIN)
			throw INTERNAL_SERVER_ERROR;
	}
	if (close(this->_fds.at(SERVER_READ_END)) == -1)
		throw INTERNAL_SERVER_ERROR;
	this->_body = resp.substr(0, this->_content_length);
	this->_content_length = _body.length();
}

void	Cgi::parse_headers(std::string &response, bool &start_body) throw()
{
	size_t						pos;
	std::vector<std::string>	tokens;

	pos = response.find("\r\n");
	while (pos != std::string::npos)
	{
		tokens = Utils::headerSplit(response.substr(0, pos));
		if (tokens.size() == 2)
		{
			std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), tolower);
			if (tokens[0] == "status")
				this->_status = std::stoi(tokens[1].substr(tokens[1].find_first_of("12345"), 3));
			if (tokens[0] == "content-type")
				this->_content_type = tokens[1];
			if (tokens[0] == "content-length")
				this->_content_length = std::stoul(tokens[1]);
			if (tokens[0] == "set-cookie")
				this->_cookies.push_back(tokens[1]);
			if (tokens[0] == "location")
				this->_location = Utils::Trim(tokens[1]);
		}
		response.erase(0, pos + 2);
		pos = response.find("\r\n");
		if (pos == 0)
		{
			response.erase(0, 2);
			start_body = true;
			break ;
		}
	}
}

void	Cgi::wait_for_child(pid_t pid) throw(int)
{
	time_t start = time(nullptr);
	int	status = 0;

	while(time(nullptr) - start <= 30)
	{
		if (waitpid(pid, &status, WNOHANG) == pid)
		{
			if ((WIFEXITED(status) == true and WEXITSTATUS(status) != EXIT_SUCCESS))
				throw BAD_GATEWAY;
			if (WIFSIGNALED(status) == true)
				throw BAD_GATEWAY;
			return ;
		}
	}
	throw GATEWAY_TIMEOUT;
}

void    Cgi::child_work(const Request& request, const Server& server, const client_info& client, const std::string& path) throw()
{
	if (dup2(this->_fds.at(CGI_WRITE_END), 1) == -1 or close(this->_fds.at(CGI_WRITE_END)))
		exit(EXIT_FAILURE);
	if (close(this->_fds.at(SERVER_READ_END)) == -1)
		exit(EXIT_FAILURE);
	if (request.method() == "POST")
	{
		if (dup2(this->_fds.at(CGI_READ_END), 0) == -1 or close(this->_fds.at(CGI_READ_END)) == -1)
			exit(EXIT_FAILURE);
		if (close(this->_fds.at(SERVER_WRITE_END)) == -1)
			exit(EXIT_FAILURE);
	}
	this->setup_environment(request, server, client, path);
	char *env[this->_env.size() + 1];
	for (size_t index = 0; index < this->_env.size(); index++)
		env[index] = const_cast<char *>(this->_env.at(index).c_str());
	env[this->_env.size()] = nullptr;
	std::string	filename = Utils::getFileName(path);
	std::string	dirname = Utils::getDirName(path);
	char *argv[3] = {const_cast<char *>(this->_cgi.c_str()), const_cast<char *>(filename.c_str()), nullptr};
	if (chdir(dirname.c_str()) == -1)
		exit(EXIT_FAILURE);
	if (execve(argv[0], argv, env) == -1)
		exit(EXIT_FAILURE);
}

void	Cgi::setup_environment(const Request& request, const Server& server, const client_info& client, const std::string& path) throw()
{
	this->_env.push_back("SERVER_SOFTWARE=WebServ/1.0.0");
	this->_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->_env.push_back(std::string("SERVER_NAME=") + server.serverNames.front());
	this->_env.push_back(std::string("SERVER_PORT=") + request.port());
	this->_env.push_back("GETEWAY_INTERFACE=CGI/1.0");
	this->_env.push_back("REDIRECT_STATUS=200");
	this->_env.push_back(std::string("REMOTE_ADDR=") + std::string(inet_ntoa(client.sin_addr)));
	this->_env.push_back(std::string("CONTENT_LENGTH=") + std::to_string((request.contentLength())));
	this->_env.push_back(std::string("CONTENT_TYPE=") + request.contentType());
	this->_env.push_back(std::string("QUERY_STRING=") + request.query());
	this->_env.push_back(std::string("REQUEST_METHOD=") + request.method());
	if (not request.cookies().empty())
		this->_env.push_back(std::string("HTTP_COOKIE=") + request.cookies());
	this->_env.push_back(std::string("SCRIPT_FILENAME=") + path);
	
}

bool    Cgi::check_script_permission(const std::string& path) throw()
{
    return (access(path.c_str(), R_OK) != -1);
}


std::vector<std::string>	Cgi::getCookies() const
{
	return _cookies;
}

std::string Cgi::getContentType() const
{
	return _content_type;
}

size_t	Cgi::getContentLength() const
{
	return _content_length;
}

std::string	Cgi::getBody() const
{
	return _body;
}

short	Cgi::getStatus() const
{
	return _status;	
}

std::string Cgi::getLocation() const
{
	return _location;
}
