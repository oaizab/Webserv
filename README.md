![Webserv Banner](doc/img/webserv-banner.png)

Webserv is a lightweight and efficient web server written in C++. It provides a
robust foundation for hosting web applications and serving static content. With
its simplicity and performance, Webserv is suitable for a wide range of use
cases.

## Features

- `HTTP/1.1 Support`: Webserv supports the HTTP/1.1 protocol, allowing seamless communication with clients.
- `Static Content`: Serve static files such as HTML, CSS, JavaScript, images, and more, making it easy to host static websites.
- `Dynamic Content`: Handle dynamic content by supporting common scripting languages such as PHP, Python, and Perl.
- `Virtual Hosting`: Configure and host multiple websites using virtual hosting, allowing you to serve multiple domains from a single server.
- `Request Routing`: Define custom request routes to handle different URLs and paths based on your application's specific needs.

For more information on Webserv's features, see the [Webserv Wiki](https://github.com/oaizab/Webserv/wiki).

## Getting Started

To get started with Webserv, follow these steps:

1. Clone the Repository: Clone the Webserv repository from GitHub:

```bash
git clone https://github.com/oaizab/Webserv.git
```

2. Build the Server: Navigate to the project directory and build the server in release mode using make:

```bash
cd Webserv
make RELEASE=1
```

3. Configuration: Customize the server's configuration by modifying the webserv.conf or creating a brand new config file. Configure the server's ports, virtual hosts, and any other settings according to your requirements. For more information on configuring the server, see the [Configuration File Wiki](https://github.com/oaizab/Webserv/wiki/Configuration-File)

4. Start the Server: Launch the server by running the built executable, you can use the default config file (config/webserv.conf) or specify a custom config file:

```bash
./webserv # Uses the default config file (config/webserv.conf)
./webserv /path/to/your/config-file.conf # Uses a custom config file
```

5. Access the Server: You can now access the server by opening a web browser and navigating to `http://localhost:port`, where port is the configured port number in the configuration file (Assuming the web server is configured to listen on localhost).

Here's a quick example of a simple configuration file that serves static content:

```conf
server {
	listen 8080;
	server_name localhost;

	location / {
		root /path/to/your/static/files;
	}
}
```

## Contact

If you have any questions, suggestions, or feedback, feel free to reach out to the project team.