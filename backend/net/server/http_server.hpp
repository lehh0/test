#ifndef BACKEND_NET_HTTP_SERVER_HPP
#define BACKEND_NET_HTTP_SERVER_HPP

#include <boost/asio/awaitable.hpp>
#include <filesystem>

#include <net/i_connection_handler.hpp>

namespace net::server {

class HttpServer 
{
public:
    struct Config 
    {
        std::filesystem::path docRoot;

        std::wstring serverUrl;
        std::wstring hostName;
        uint16_t port;
    };

    HttpServer(HttpServer::Config cfg,
        std::shared_ptr<IConnectionHandler> connectionsHandler);

    boost::asio::awaitable<void> Run();

private:
    Config m_config;
    std::shared_ptr<IConnectionHandler> m_connectionsHandler;
};

}// namespace net::server

#endif// BACKEND_NET_HTTP_SERVER_HPP