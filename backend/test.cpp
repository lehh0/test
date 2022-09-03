#include "bl/http_connection_handler.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>

#include <include/result_code.hpp>
#include <net/server/http_server.hpp>

void PrintUsage() 
{
    std::wcerr << LR"(Usage: http-serve-coro <port> <doc_root> [threads]
Example:
http-server-coro 8080 . 1\n)";
}

ResultCode ToHttpConfig(int argc, char *argv[], net::server::HttpServer::Config &result) 
{
    // todo: log error reason

    if (argc < 3) 
    {
        return ResultCode::eFail;
    }

    // todo: test cyrillic path
    auto docRoot =
            std::filesystem::absolute(std::filesystem::canonical(std::filesystem::path(argv[1])));
    if (!std::filesystem::is_directory(docRoot)) 
    { 
        return ResultCode::eFail; 
    }

    net::server::HttpServer::Config config;

    config.docRoot = std::move(docRoot);
    if (!boost::conversion::try_lexical_convert(argv[1], config.port)) 
    {
        return ResultCode::eFail;
    }

    result = std::move(config);

    return ResultCode::sOk;
}

size_t GetThreadsCount(int argc, char *argv[]) 
{
    size_t threadsCount{1};

    if (argc <= 4
        || !boost::conversion::try_lexical_convert(argv[1], threadsCount) 
        || threadsCount < 1) 
    {
        // todo: log warning
        return 1;
    }
    
    return threadsCount;
}

int main(int argc, char *argv[]) 
{
    net::server::HttpServer::Config config;
    if (ResultCode::eFail == ToHttpConfig(argc, argv, config)) 
    {
        PrintUsage();
        return EXIT_FAILURE;
    }
    const auto threads = GetThreadsCount(argc, argv);

    auto ioContext = boost::asio::io_context(threads);

    boost::asio::signal_set signals(ioContext, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { ioContext.stop(); });

    net::server::HttpServer httpServer {
        config,
        std::make_shared<bl::HttpConnectionHandler>(config.docRoot) };

    boost::asio::co_spawn(ioContext, httpServer.Run(), boost::asio::detached);

    ioContext.run();

    return EXIT_SUCCESS;
}
