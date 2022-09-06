#include <iostream>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <spdlog/spdlog.h>

#include <include/result_code.hpp>
#include <include/logging.h>
#include <net/server/http_server.hpp>
#include <bl/http_connection_handler.hpp>

void PrintUsage() 
{
    spdlog::info(R"(Usage: http-serve-coro <port> <doc_root> [threads]
Example:
http-server-coro 8080 . 1\n)");
}

ResultCode ToHttpConfig(int argc, char *argv[], net::server::HttpServer::Config &result) 
{
    if (argc < 3) 
    {
        spdlog::error("invalid args count");
        return ResultCode::eFail;
    }

    // todo: test cyrillic path
    auto docRoot = absolute(canonical(std::filesystem::path(argv[2])));
    if (!is_directory(docRoot)) 
    {
        spdlog::error("<doc_root> value [{}] isn't directory", docRoot.string());
        return ResultCode::eFail; 
    }

    net::server::HttpServer::Config config;

    config.docRoot = std::move(docRoot);
    if (!boost::conversion::try_lexical_convert(argv[1], config.port)) 
    {
        spdlog::error("<port> can't convert port value [{}] to number", argv[1]);
        return ResultCode::eFail;
    }

    result = std::move(config);
    return ResultCode::sOk;
}

size_t GetThreadsCount(int argc, char *argv[]) 
{
    size_t threadsCount{1};

    if (argc < 4
        || !boost::conversion::try_lexical_convert(argv[1], threadsCount) 
        || threadsCount < 1) 
    {
        spdlog::warn("[threads] threads count set to 1");
        return 1;
    }

    spdlog::info("[threads] threads count set to [{}]", threadsCount);
    return threadsCount;
}

int main(int argc, char *argv[]) 
{
    std::shared_ptr<spdlog::logger> defaultLogger;
    spdlog::set_default_logger(std::move(IntializeLogging()));

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
