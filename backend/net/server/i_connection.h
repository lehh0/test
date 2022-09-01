#ifndef BACKEND_NET_I_CONNECTION
#define BACKEND_NET_I_CONNECTION

namespace net::server {

class IConnection
{
public:
    virtual ~IConnection() = default;
};

}// namespace net::server

#endif// BACKEND_NET_I_CONNECTION