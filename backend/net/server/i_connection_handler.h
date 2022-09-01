#ifndef BACKEND_NET_I_CONNECTION_HANDLER
#define BACKEND_NET_I_CONNECTION_HANDLER

#include <memory>

#include "i_connection.h"
#include "include/result_code.h"

namespace net::server {

class IConnectionHandler
{
public:
    virtual ~IConnectionHandler() = default;
    virtual ResultCode Handle(std::shared_ptr<IConnection>) = 0;
};

}// namespace net::server

#endif// BACKEND_NET_I_CONNECTION_HANDLER