#ifndef __API_HANDLER_H__
#define __API_HANDLER_H__

#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast/http.hpp>
// #include <boost/json.hpp>
#include <functional>
#include <string_view>

#include "application.h"
#include "model/player_list.h"
#include "json_parser.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

using StringResponse = http::response<http::string_body>;
using StringRequest = http::request<http::string_body>;

class ApiHandler {
 public:
  ApiHandler(Application& app, const StringRequest& req);

  StringResponse Response();

 private:
  Application& m_app;
  const StringRequest& m_req;
  const std::string m_target;

  StringResponse MapRequest();

  StringResponse PlayerJoinRequest();

  StringResponse PlayerListRequest();

  StringResponse GetGameState();

  StringResponse PostAction();
  StringResponse PostTick();

  /**
   * @brief
   * @return Token if can extruct
   */
  std::optional<model::Token> TryExtractToken();

  StringResponse ExecuteAuthorized(std::function<StringResponse(model::Player& player)> action);

  /**
   * @brief Create Response from
   *
   * @param status netork status
   * @param val json value
   * @param cache_control
   * @param allow
   * @return StringResponse
   */
  StringResponse MakeJsonResponse(const http::status& status,
                                  const json::value& val,
                                  std::string_view cache_control = "no-cache"sv,
                                  std::string_view allow = std::string_view());

  
};

}  // namespace http_handler

#endif  // __API_HANDLER_H__