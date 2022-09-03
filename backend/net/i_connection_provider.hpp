#ifndef BACKEND_NET_I_CONNECTION_PROVIDER_HPP
#define BACKEND_NET_I_CONNECTION_PROVIDER_HPP

#include "i_connection_handler.hpp"

namespace net::server {

class IConnectionProvider 
{
public:
    virtual ~IConnectionProvider() = default;

    virtual ResultCode AcceptConnection(std::shared_ptr<IConnectionHandler>&) = 0;
};

}// namespace net::server

#endif// BACKEND_NET_I_CONNECTION_PROVIDER_HPP