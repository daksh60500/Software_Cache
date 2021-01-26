#include "cache_client.hpp"

Client::Client(std::string host, std::string port): host_(host), port_(port) {
}

void Client::connect() {
    try {//normal method threw an uncaught exception, so used try catch
        auto const res = resolver.resolve(host_, port_);
        stream.connect(res);
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void Client::disconnect() {
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    if(ec && ec != beast::errc::not_connected)//if can't disconnect
        throw beast::system_error{ec};
}

bool Client::process(http::request<http::empty_body> request) {
    http::response<http::string_body> res;
    connect();
    http::write(stream, request);
    if (request.method() == http::verb::get || request.method() == http::verb::delete_ || request.method() == http::verb::post) {
        http::read(stream, buffer, res);
        disconnect();
        return res.result() == http::status::ok;
    }
    return true;
}

Client::~Client() = default;
