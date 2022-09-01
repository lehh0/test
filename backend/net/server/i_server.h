#ifndef BACKEND_NET_I_SERVER
#define BACKEND_NET_I_SERVER

#include "i_connection_handler.h"

namespace net::server {

class IServer 
{
public:
    virtual ~IServer() = default;

    virtual ResultCode AcceptConnection(std::shared_ptr<IConnectionHandler>) = 0;
    virtual ResultCode GetAvailableBytes(std::size_t&) = 0;
};

}// namespace net::server

#endif// BACKEND_NET_I_SERVER