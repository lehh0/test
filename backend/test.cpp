#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/version.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

beast::string_view mime_type(beast::string_view path) {
    using beast::iequals;
    const auto ext = std::invoke([&path] {
      if (const auto pos = path.rfind("."); pos != beast::string_view::npos) {
          return path.substr(pos);
      }
      return beast::string_view();
    });

    if (iequals(ext, ".htm")) { return "text/html"; }
    if (iequals(ext, ".html")) { return "text/html"; }
    if (iequals(ext, ".php")) { return "text/html"; }
    if (iequals(ext, ".css")) { return "text/css"; }
    if (iequals(ext, ".txt")) { return "text/plain"; }
    if (iequals(ext, ".js")) { return "application/javascript"; }
    if (iequals(ext, ".json")) { return "application/json"; }
    if (iequals(ext, ".xml")) { return "application/xml"; }
    if (iequals(ext, ".swf")) { return "application/x-shockwave-flash"; }
    if (iequals(ext, ".flv")) { return "video/x-flv"; }
    if (iequals(ext, ".png")) { return "image/png"; }
    if (iequals(ext, ".jpe")) { return "image/jpeg"; }
    if (iequals(ext, ".jpeg")) { return "image/jpeg"; }
    if (iequals(ext, ".jpg")) { return "image/jpeg"; }
    if (iequals(ext, ".gif")) { return "image/gif"; }
    if (iequals(ext, ".bmp")) { return "image/bmp"; }
    if (iequals(ext, ".ico")) { return "image/vnd.microsoft.icon"; }
    if (iequals(ext, ".tiff")) { return "image/tiff"; }
    if (iequals(ext, ".tif")) { return "image/tiff"; }
    if (iequals(ext, ".svg")) { return "image/svg+xml"; }
    if (iequals(ext, ".svgz")) { return "image/svg+xml"; }
    return "application/text";
}
std::string path_cat(beast::string_view base, beast::string_view path) {
    if (base.empty()) { return std::string(path); }

    auto result = std::string(base);

#ifdef BOOST_MSVC
    constexpr char path_separator = '\\';
    if (result.back() == path_separator) { result.resize(result.size() - 1); }
    result.append(path.data(), path.size());
    for (auto &c: result) { c = (c == '/') ? path_separator : c; }
#else
    constexpr char path_separator = '/';
    if (result.back() == path_separator) { result.resize(result.size() - 1); }
    result.append(path.data(), path.size());
#endif
    return result;
}

void fail(beast::error_code ec, const char *what);


void do_listen(asio::io_context &ioc, tcp::endpoint endpoint,
               const std::shared_ptr<const std::string> &doc_root, asio::yield_context yield);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: http-serve-coro <port> <doc_root> <threads>\n"
                  << "Example:\n"
                  << "http-server-coro 8080 . 1\n";
        return EXIT_FAILURE;
    }

    const auto port = boost::lexical_cast<unsigned short>(argv[1]);
    const auto doc_root = std::make_shared<std::string>(argv[2]);
    const auto threads = std::max(1, boost::lexical_cast<int>(argv[3]));

    auto ioc = asio::io_context(threads);

    asio::spawn(ioc, [&ioc, ep = tcp::endpoint(asio::ip::tcp::v4(), port), &doc_root](
                             asio::yield_context yield) { do_listen(ioc, ep, doc_root, yield); });

    ioc.run();

    return EXIT_SUCCESS;
}
