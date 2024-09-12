#include "http_server.h"

namespace http_server {
    void ReportError(beast::error_code ec, std::string_view what) {
        boost::json::value data{
                {"code", ec.value()},
                {"text", ec.message()},
                {"where", what}
        };
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, data) << "server exited";

        std::cerr << what << ": "sv << ec.message() << std::endl;
    }

    void SessionBase::Run() {
        net::dispatch(stream_.get_executor(),
            beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

    SessionBase::SessionBase(tcp::socket&& socket)
        : stream_(std::move(socket)) {
    }

    void SessionBase::Read() {
        using namespace std::literals;
        // Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
        request_ = {};
        stream_.expires_after(30s);
        // Считываем request_ из stream_, используя buffer_ для хранения считанных данных
        http::async_read(stream_, buffer_, request_,
            // По окончании операции будет вызван метод OnRead
            beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
    }
    
    void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
        if (ec == http::error::end_of_stream) {
            // Нормальная ситуация - клиент закрыл соединение
            return Close();
        }
        if (ec) {
            return ReportError(ec, "read"sv);
        }
        start_time_ = std::chrono::steady_clock::now();

        boost::json::value data{ 
            {"ip", GetClientIp()}, 
            {"URI", request_.target().to_string()}, 
            {"method", request_.method_string().to_string()}};
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, data) << "request received";

        HandleRequest(std::move(request_));
    }

    void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
        if (ec) {
            return ReportError(ec, "write"sv);
        }

        if (close) {
            // Семантика ответа требует закрыть соединение
            return Close();
        }

        // Считываем следующий запрос
        Read();
    }

    void SessionBase::Close() {
        stream_.socket().shutdown(tcp::socket::shutdown_send);
    }

    std::string SessionBase::GetClientIp() const {
        return stream_.socket().remote_endpoint().address().to_string();
    }


}  // namespace http_server
