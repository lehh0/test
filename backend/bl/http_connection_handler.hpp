#ifndef BACKEND_BL_HTTP_CONNECTION_HANDLER_HPP
#define BACKEND_BL_HTTP_CONNECTION_HANDLER_HPP

#include <filesystem>
#include <net/i_connection_handler.hpp>

namespace bl {

class HttpConnectionHandler 
    : public net::server::IConnectionHandler
{
public:
    HttpConnectionHandler(std::filesystem::path docRoot);
    ~HttpConnectionHandler() override;

    boost::asio::awaitable<void> Handle(boost::asio::ip::tcp::socket) override;

private:
    std::filesystem::path m_docRoot;
};

}// namespace bl

#endif// BACKEND_BL_HTTP_CONNECTION_HANDLER_HPP