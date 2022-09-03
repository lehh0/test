#include "http_connection_handler.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>

#include <include/as_result.hpp>

namespace bl {

namespace {

std::pair<std::filesystem::path, std::string_view> ParseTarget(boost::beast::string_view targetBeast)
{
    std::string_view target(targetBeast.data(), targetBeast.size());
    size_t queryStringBegin = target.find('?');

    if (queryStringBegin != boost::beast::string_view::npos) 
    {
        auto const path = target.substr(0, queryStringBegin);
        target.remove_prefix(queryStringBegin);

        return {{path, std::filesystem::path::format::generic_format}, target};
    }
    return {{target, std::filesystem::path::format::generic_format}, {}};
}

} // anonymous namespace

HttpConnectionHandler::HttpConnectionHandler(std::filesystem::path docRoot) 
    : m_docRoot(std::move(docRoot))
{
    // todo: pre check m_docRoot is valid
}

HttpConnectionHandler::~HttpConnectionHandler() = default;

boost::asio::awaitable<void> HttpConnectionHandler::Handle(boost::asio::ip::tcp::socket socket)
{
    boost::beast::tcp_stream stream{std::move(socket)};
    boost::beast::flat_buffer buffer;

    for (;;)
    {
        boost::beast::http::request<boost::beast::http::string_body> request;

        // todo: customize timeouts
        stream.expires_after(std::chrono::seconds(30));
        auto result = co_await boost::beast::http::async_read(stream, buffer, request,
            tools::as_result(boost::asio::use_awaitable));// TODO: use_async_result

        if (!result) 
        {
            // todo: log error
            break;
        }

        // todo: log "Got request: {} {}", req.method_string(), req.target()

        auto [path, queryString] = ParseTarget(request.target());

    }
}

}// namespace bl