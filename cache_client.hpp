#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/format.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Client {
    private:
        std::string host_, port_;
        beast::flat_buffer buffer;
        net::io_context ioc;
        tcp::resolver resolver{ioc};
        beast::tcp_stream stream{ioc};
        
    public:
        Client(std::string host, std::string port);
        void connect();
        void disconnect();
        bool process(http::request<http::empty_body> request);
        ~Client();
};
