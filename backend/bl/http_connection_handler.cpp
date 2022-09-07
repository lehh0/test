#include "http_connection_handler.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

#include <include/as_result.hpp>
#include <include/contract.hpp>

namespace bl {

namespace {

std::pair<std::filesystem::path, std::string_view> ParseTarget(boost::beast::string_view targetBeast)
{
    std::string_view target(targetBeast.data(), targetBeast.size());
    size_t queryStringBegin = target.find('?');

    if (queryStringBegin != boost::beast::string_view::npos) 
    {
        TRACE_DEBUG(fmt::format("query string found [{}])", queryStringBegin));

        auto const path = target.substr(0, queryStringBegin);
        target.remove_prefix(queryStringBegin);

        return {{path, std::filesystem::path::format::generic_format}, target};
    }

    TRACE_DEBUG(" query string not found");
    return {{target, std::filesystem::path::format::generic_format}, {}};
}

} // anonymous namespace

HttpConnectionHandler::HttpConnectionHandler(std::filesystem::path docRoot) 
    : m_docRoot(std::move(docRoot))
{
    CHECK_SUCCEEDED(std::filesystem::is_directory(m_docRoot), 
        "can't initialize docRoot 'cause it isn't directory")
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
            tools::as_result(boost::asio::use_awaitable));// todo: use_async_result

        if (!result) 
        {
            TRACE_ERROR(fmt::format(
                    " can't async_read from stream [{}]",
                    result.error().to_string()))
            break;
        }

        auto method = request.method_string();
        auto target = request.target();
        TRACE_INFO(fmt::format(" got request [{}] [{}]",
                    std::string_view(method.data(), method.size()),
                    std::string_view(target.data(), target.size())));

        auto [path, queryString] = ParseTarget(request.target());
        TRACE_INFO(fmt::format(" path [{}] query [{}]", path.string(), queryString));

        // todo: return requested file
    }
}

}// namespace bl