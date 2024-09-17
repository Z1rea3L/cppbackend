#include "http_server.h"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {

void ReportError(beast::error_code ec, std::string_view what) {
    std::cerr << what << ": "sv << ec.message() << std::endl;
}

void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
    if (ec) {
        return logger::LogNetError(ec.value(), ec.message(), "write"sv);
    }

    if (close) {
        return Close();
    }
    Read();
}

void SessionBase::Read() {
    using namespace std::literals;

    request_ = {};
    stream_.expires_after(30s);

    http::async_read(stream_, 
                     buffer_, request_,

                     beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
}

void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
    using namespace std::literals;
        req_time_ = boost::posix_time::microsec_clock::local_time();
    
    if (ec == http::error::end_of_stream) {
        return Close();
    }
    if (ec) {
        return logger::LogNetError(ec.value(), ec.message(), "read"sv);
    }
    logger::LogRequest(stream_.socket().remote_endpoint().address().to_string(), 
                            request_.target(), 
                            MethodToString(request_.method()));
    HandleRequest(std::move(request_));
}

void SessionBase::Close() {
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    if (ec) {
        return ReportError(ec, "close"sv);
    }
}

std::string SessionBase::MethodToString(http::verb verb) {
    switch ( verb ) {
        case http::verb::get:  return "GET";
        case http::verb::head: return "HEAD";
        case http::verb::post: return "POST";
        case http::verb::put:  return "PUT";
    }
    return "UNKNOWN";
}
    
}  // namespace http_server
