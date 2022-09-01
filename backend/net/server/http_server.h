#ifndef BACKEND_NET_SERVER
#define BACKEND_NET_SERVER

#include "i_server.h"

namespace net::server {

    class HttpServer
        : public IServer 
    {
    public:
        ~HttpServer() override = default;

        HttpServer();

        ResultCode AcceptConnection(std::shared_ptr<IConnectionHandler>) override;
        ResultCode GetAvailableBytes(std::size_t&) override;
    };

}// namespace net::server

#endif// BACKEND_NET_SERVER