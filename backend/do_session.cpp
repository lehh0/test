#include <boost/asio/spawn.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
//
//namespace beast = boost::beast;
//namespace http = beast::http;
//namespace asio = boost::asio;
//using tcp = asio::ip::tcp;
//
//void fail(beast::error_code ec, const char *what);
//
//beast::string_view mime_type(beast::string_view path);
//
//std::string path_cat(beast::string_view base, beast::string_view path);
//
//class Response {
//public:
//    virtual ~Response() = default;
//
//public:
//    virtual beast::error_code AsyncWrite(beast::tcp_stream &stream, asio::yield_context yield) = 0;
//
//    [[nodiscard]] virtual bool KeepAlive() const = 0;
//};
//
//namespace details {
//
//    template<typename Response>
//    class ResponseT {
//    public:
//        explicit ResponseT(Response response) : _response(std::move(response)) {}
//
//    public:
//        beast::error_code AsyncWriteImpl(beast::tcp_stream &stream, asio::yield_context yield) {
//            auto ec = beast::error_code();
//            http::async_write(stream, _response, yield[ec]);
//            return ec;
//        }
//
//        [[nodiscard]] bool KeepAliveImpl() const { return _response.keep_alive(); }
//
//    private:
//        Response _response;
//    };
//}// namespace details
//
//class StringBodyResponse : public Response,
//                           public details::ResponseT<http::response<http::string_body>> {
//public:
//    using details::ResponseT<http::response<http::string_body>>::ResponseT;
//
//public:
//    beast::error_code AsyncWrite(beast::tcp_stream &stream, asio::yield_context yield) override {
//        return this->AsyncWriteImpl(stream, yield);
//    }
//    [[nodiscard]] bool KeepAlive() const override { return this->KeepAliveImpl(); }
//};
//
//class EmptyBodyResponse : public Response,
//                          public details::ResponseT<http::response<http::empty_body>> {
//public:
//    using details::ResponseT<http::response<http::empty_body>>::ResponseT;
//
//public:
//    beast::error_code AsyncWrite(beast::tcp_stream &stream, asio::yield_context yield) override {
//        return this->AsyncWriteImpl(stream, yield);
//    }
//
//    [[nodiscard]] bool KeepAlive() const override { return this->KeepAliveImpl(); }
//};
//
//class FileBodyResponse : public Response,
//                         public details::ResponseT<http::response<http::file_body>> {
//public:
//    using details::ResponseT<http::response<http::file_body>>::ResponseT;
//
//public:
//    beast::error_code AsyncWrite(beast::tcp_stream &stream, asio::yield_context yield) override {
//        return this->AsyncWriteImpl(stream, yield);
//    }
//
//    [[nodiscard]] bool KeepAlive() const override { return this->KeepAliveImpl(); }
//};
//template<class Body, class Allocator>
//std::unique_ptr<Response> handle_request(beast::string_view doc_root,
//                                         http::request<Body, http::basic_fields<Allocator>> &&req) {
//    const auto bad_request = [&req](beast::string_view why) {
//        auto res = http::response<http::string_body>(http::status::bad_request, req.version());
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = std::string(why);
//        res.prepare_payload();
//        return std::make_unique<StringBodyResponse>(std::move(res));
//    };
//
//    const auto not_found = [&req](beast::string_view target) {
//        auto res = http::response<http::string_body>(http::status::not_found, req.version());
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "The resource '" + std::string(target) + "' was not found.";
//        res.prepare_payload();
//        return std::make_unique<StringBodyResponse>(std::move(res));
//    };
//
//    const auto server_error = [&req](beast::string_view what) {
//        auto res = http::response<http::string_body>(http::status::not_found, req.version());
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, "text/html");
//        res.keep_alive(req.keep_alive());
//        res.body() = "An error occurred: '" + std::string(what) + "'";
//        res.prepare_payload();
//        return std::make_unique<StringBodyResponse>(std::move(res));
//    };
//
//    if (req.method() != http::verb::get && req.method() != http::verb::head) {
//        return bad_request("Unknown HTTP-method");
//    }
//
//    std::cerr << "req.target = " << req.target() << std::endl;
//    if (req.target().empty() || req.target()[0] != '/' ||
//        req.target().find("..") != beast::string_view::npos) {
//        return bad_request("Illegal request-target");
//    }
//
//    auto path = path_cat(doc_root, req.target());
//    if (req.target().back() == '/') { path.append("index.html"); }
//
//    auto ec = beast::error_code();
//    auto body = http::file_body::value_type();
//    body.open(path.c_str(), beast::file_mode::scan, ec);
//
//    if (ec == beast::errc::no_such_file_or_directory) { return not_found(req.target()); }
//
//    if (ec) { return server_error(ec.message()); }
//
//    const auto size = body.size();
//
//    if (req.method() == http::verb::head) {
//        auto res = http::response<http::empty_body>(http::status::ok, req.version());
//        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//        res.set(http::field::content_type, mime_type(path));
//        res.content_length(size);
//        res.keep_alive(req.keep_alive());
//        return std::make_unique<EmptyBodyResponse>(std::move(res));
//    }
//
//    auto res =
//            http::response<http::file_body>(std::piecewise_construct, std::tuple(std::move(body)),
//                                            std::tuple(http::status::ok, req.version()));
//    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//    res.set(http::field::content_type, mime_type(path));
//    res.content_length(size);
//    res.keep_alive(req.keep_alive());
//    return std::make_unique<FileBodyResponse>(std::move(res));
//}
//
//void do_session(beast::tcp_stream &stream, const std::shared_ptr<const std::string> &doc_root,
//                asio::yield_context yield) {
//    auto ec = beast::error_code();
//    auto buffer = beast::flat_buffer();
//
//    for (;;) {
//        auto req = http::request<http::string_body>();
//        http::async_read(stream, buffer, req, yield[ec]);
//        if (ec == http::error::end_of_stream) { break; }
//        if (ec) { return fail(ec, "read"); }
//
//        auto msg = handle_request(*doc_root, std::move(req));
//
//        auto keep_alive = msg->KeepAlive();
//
//        if (ec = msg->AsyncWrite(stream, yield); ec) { return fail(ec, "write"); }
//
//        if (!keep_alive) { break; }
//    }
//
//    stream.socket().shutdown(tcp::socket::shutdown_send, ec);
//}
