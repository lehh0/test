#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <iostream>

using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace asio = boost::asio;

void do_session(beast::tcp_stream &stream, const std::shared_ptr<const std::string> &doc_root,
                asio::yield_context yield);

void fail(beast::error_code ec, const char *what);

void do_listen(boost::asio::io_context &ioc, tcp::endpoint endpoint,
               const std::shared_ptr<const std::string> &doc_root,
               boost::asio::yield_context yield) {
    auto ec = beast::error_code();

    auto acceptor = tcp::acceptor(ioc);
    acceptor.open(endpoint.protocol(), ec);
    if (ec) { return fail(ec, "open"); }

    acceptor.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec) { return fail(ec, "set_option"); }

    acceptor.bind(endpoint, ec);
    if (ec) { return fail(ec, "bind"); }

    acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) { return fail(ec, "listen"); }

    for (;;) {
        auto socket = tcp::socket(ioc);
        acceptor.async_accept(socket, yield[ec]);
        if (ec) { return fail(ec, "accept"); }
        asio::spawn(acceptor.get_executor(), [stream = beast::tcp_stream(std::move(socket)),
                                              doc_root](const asio::yield_context &yield) mutable {
            do_session(stream, doc_root, yield);
        });
    }
}

void fail(beast::error_code ec, const char *what) {
    std::cerr << what << ": " << ec.message() << "\n";
}
