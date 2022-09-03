#ifndef BACKEND_NET_I_CONNECTION_HANDLER_HPP
#define BACKEND_NET_I_CONNECTION_HANDLER_HPP

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>

#include <include/result_code.hpp>

namespace net::server {

class IConnectionHandler
{
public:
    virtual ~IConnectionHandler() = default;
    // todo: boost::asio::ip::tcp::socket to abstract IConnection?
    virtual boost::asio::awaitable<void> Handle(boost::asio::ip::tcp::socket) = 0;
};

}// namespace net::server

#endif// BACKEND_NET_I_CONNECTION_HANDLER_HPP