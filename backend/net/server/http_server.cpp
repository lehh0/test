#include "http_server.hpp"

#include <format>
#include <string_view>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

namespace net::server {

HttpServer::HttpServer(HttpServer::Config config, 
    std::shared_ptr<IConnectionHandler> connectionsHandler)
    : m_config(std::move(config))
    , m_connectionsHandler(std::move(connectionsHandler))
{
    // todo: pre check m_config is valid
    // todo: check m_connectionsHandler is not null
}

boost::asio::awaitable<void> HttpServer::Run()
{
    auto executor = co_await boost::asio::this_coro::executor;
    auto acceptor = boost::asio::ip::tcp::acceptor { 
        executor, 
        { boost::asio::ip::tcp::v4(), m_config.port }};

    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.set_option(boost::asio::ip::tcp::acceptor::keep_alive(true));

    // todo: sync just in case?
    m_config.serverUrl = std::format(L"http://{}:{}", m_config.hostName, m_config.port);

    // todo: log info: Server listening on {}", m_config.serverUrl
    
    for (;;) 
    {
        auto socket = co_await acceptor.async_accept(boost::asio::use_awaitable);
        boost::asio::co_spawn(
            executor,
            [handler = m_connectionsHandler, socket = std::move(socket)]() mutable
            {
                return handler->Handle(std::move(socket));
            },
            boost::asio::detached);
    }
}

}// namespace net::server
