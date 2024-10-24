#pragma once
namespace event_logger {

void InitLogger();

void LogStartServer(const std::string& address, unsigned int port, const std::string& message);
void LogServerEnd(const std::string& message, int code, const std::string& exception_descr="");
void LogServerRequestReceived(const std::string& uri, const std::string& http_method);
void LogServerRespondSend(int response_time, unsigned code, const std::string& content_type);
}
