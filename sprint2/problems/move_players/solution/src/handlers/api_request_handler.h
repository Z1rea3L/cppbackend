#pragma once

#include "application.h"
#include "serialization.h"

#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <optional>

#include <string>


namespace http_handler {
    namespace api_handler {
        namespace beast = boost::beast;
        namespace http = beast::http;
        using namespace std::literals;
        using namespace application;
        namespace fs = std::filesystem;

        class BadRequestBuilder {
        public:
            http::status status;
            unsigned int version;

            std::optional<bool> keep_alive;
            std::string content_type = "application/json";
            bool cache_control = false;
            std::optional<std::string> allow;

            beast::string_view code;
            beast::string_view message;

            template <typename Send>
            void HandleBadRequest(Send&& send) {
                http::response<http::string_body> response{status, version};

                response.set(http::field::content_type, content_type);

                if (keep_alive.has_value()) {
                    response.keep_alive(keep_alive.value());
                }

                if (allow.has_value()) {
                    response.set(http::field::allow, allow.value());
                }

                boost::json::object error_obj{
                    {"code", code},
                    {"message", message}
                };
                response.body() = boost::json::serialize(error_obj);
                response.content_length(response.body().size());

                if (cache_control) {
                    response.set(http::field::cache_control, "no-cache");
                }

                response.prepare_payload();
                return send(std::move(response));
            }
        };

        template <typename Body, typename Allocator, typename Send>
        class GameHandler {
        public:
            GameHandler(Application& application, http::request<Body, http::basic_fields<Allocator>>&& request, Send&& send)
                : application_(application), request_(std::move(request)), send_(std::move(send)) {

            }

            void Run() {
                auto target = request_.target();
                std::string target_str = std::string(target);


                std::string base_target = "/api/v1/game/";

                if (target_str == base_target + "join") {
                    HandleJoinGame();
                }
                else if (target_str == base_target + "players") {
                    HandleGetPlayers();
                }
                else if (target_str == base_target + "state") {
                    HandleState();
                }
                else if (target_str == base_target + "player/action") {

                }
            }

        private:
            void HandleJoinGame() {
                if (http::verb::post != request_.method()) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::method_not_allowed;
                    handler.allow = "POST";
                    handler.cache_control = true;
                    handler.code = "invalidMethod";
                    handler.message = "Only POST method is expected";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                boost::json::value request_body;
                std::string map_id;
                std::string user_name;

                try {
                    request_body = boost::json::parse(request_.body());
                    map_id = request_body.as_object()["mapId"].as_string();;
                    user_name = request_body.as_object()["userName"].as_string();
                }
                catch (...) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::bad_request;
                    handler.cache_control = true;
                    handler.code = "invalidArgument";
                    handler.message = "Join game request parse error";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                auto* map = application_.GetMap(map_id);

                if (user_name.empty()) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::bad_request;
                    handler.cache_control = true;
                    handler.code = "invalidArgument";
                    handler.message = "Invalid name";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                if (!map) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::not_found;
                    handler.cache_control = true;
                    handler.code = "mapNotFound";
                    handler.message = "Map not found";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                auto data = application_.JoinGame(map, user_name);

                return send_(std::move(MakeJoinGameResponse(data)));
            }

            http::response<http::string_body> MakeJoinGameResponse(std::pair<PlayerToken, size_t>& data) {
                http::response<http::string_body> response;

                response.result(http::status::ok);
                response.version(request_.version());
                response.set(http::field::content_type, "application/json");

                boost::json::object obj{
                    {"authToken", data.first.ToString()},
                    {"playerId", data.second}
                };

                response.body() = boost::json::serialize(obj);
                response.content_length(response.body().size());
                response.set(http::field::cache_control, "no-cache");

                response.prepare_payload();

                return response;
            }

            Player* AuthenticatePlayer() {
                auto auth_field = request_[http::field::authorization];
                if (auth_field.empty() || !auth_field.starts_with("Bearer ")) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::unauthorized;
                    handler.cache_control = true;
                    handler.code = "invalidToken";
                    handler.message = "Authorization header is missing";

                    handler.HandleBadRequest(std::move(send_));
                    return {};
                }

                std::string auth_token = std::string(auth_field.substr(7));
                if (auth_token.size() != 32) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::unauthorized;
                    handler.cache_control = true;
                    handler.code = "invalidToken";
                    handler.message = "Token has an invalid length";

                    handler.HandleBadRequest(std::move(send_));
                    return {};
                }


                auto token = application::player::PlayerToken::FromString(auth_token);

                return application_.GetPlayer(token);
            }

            std::vector<Player*> GetPlayersDefaultHandler() {
                if (request_.method() != http::verb::head && request_.method() != http::verb::get) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::method_not_allowed;
                    handler.allow = "GET, HEAD";
                    handler.cache_control = true;
                    handler.code = "invalidMethod";
                    handler.message = "Invalid method";

                    handler.HandleBadRequest(std::move(send_));
                    return {};
                }

                auto* player = AuthenticatePlayer();

                if (!player) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::unauthorized;
                    handler.cache_control = true;
                    handler.code = "unknownToken";
                    handler.message = "Player token has not been found";

                    handler.HandleBadRequest(std::move(send_));
                    return {};
                }

                return player->GetSession()->GetPlayersVector();
            }

            void HandleGetPlayers() {
                std::vector<Player*> players = GetPlayersDefaultHandler();

                if (players.empty()) {
                    return;
                }

                http::response<http::string_body> response;
                response.result(http::status::ok);
                response.version(request_.version());
                response.set(http::field::content_type, "application/json");
                response.set(http::field::cache_control, "no-cache");
                response.keep_alive(request_.keep_alive());

                if (request_.method() != http::verb::head) {
                    boost::json::object json_body;

                    for (const auto& player : players) {
                        json_body[std::to_string(player->GetId())] = boost::json::object({ {"name", player->GetName()} });
                    }

                    response.body() = boost::json::serialize(json_body);
                    response.content_length(response.body().size());
                }
                else {
                    response.content_length(0);
                }

                response.prepare_payload();

                return send_(std::move(response));
            }

            void HandleState() {
                std::vector<Player*> players = GetPlayersDefaultHandler();

                if (players.empty()) {
                    return;
                }

                http::response<http::string_body> response;
                response.result(http::status::ok);
                response.version(request_.version());
                response.set(http::field::content_type, "application/json");
                response.set(http::field::cache_control, "no-cache");
                response.keep_alive(request_.keep_alive());

                if (request_.method() != http::verb::head) {
                    boost::json::object players_object;

                    for (const auto& player : players) {
                        boost::json::object player_body;
                        
                        Coordinates position = player->GetPosition();
                        Speed speed = player->GetSpeed();

                        player_body["pos"] = boost::json::array({ position.x, position.y });

                        player_body["speed"] = boost::json::array({ speed.x, speed.y });

                        std::string string_direction;

                        switch (player->GetDirection()) {
                        case Direction::NORTH:
                            string_direction = "U";
                            break;
                        case Direction::SOUTH:
                            string_direction = "D";
                            break;
                        case Direction::EAST:
                            string_direction = "R";
                            break;
                        case Direction::WEST:
                            string_direction = "L";
                            break;
                        }

                        player_body["dir"] = boost::json::value(string_direction);

                        players_object[std::to_string(player->GetId())] = boost::json::value(player_body);
                    }

                    boost::json::object json_body;

                    json_body["players"] = boost::json::value(players_object);

                    response.body() = boost::json::serialize(json_body);
                    response.content_length(response.body().size());
                }
                else {
                    response.content_length(0);
                }

                response.prepare_payload();

                return send_(std::move(response));
            }

            void HandlePlayerAction() {
                if (request_[http::field::content_type] != "application/json") {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::bad_request;
                    handler.cache_control = true;
                    handler.code = "invalidArgument";
                    handler.message = "Invalid content type";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                if (http::verb::post != request_.method()) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::method_not_allowed;
                    handler.allow = "POST";
                    handler.cache_control = true;
                    handler.code = "invalidMethod";
                    handler.message = "Only POST method is expected";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                auto* player = AuthenticatePlayer();

                boost::json::value request_body;
                request_body = boost::json::parse(request_.body());
                char move_char;

                try {
                    request_body = boost::json::parse(request_.body());
                    move_char = request_body.as_object().at("move").as_string();
                }
                catch (const std::exception&) {
                    BadRequestBuilder handler;
                    handler.version = request_.version();
                    handler.status = http::status::bad_request;
                    handler.cache_control = true;
                    handler.code = "invalidArgument";
                    handler.message = "Failed to parse action";

                    handler.HandleBadRequest(std::move(send_));
                    return;
                }

                switch (move_char) {
                case 'U': 
                    player->SetDirection(Direction::NORTH);
                    break;
                case 'D':
                    player->SetDirection(Direction::SOUTH);
                    break;
                case 'R':
                    player->SetDirection(Direction::EAST);
                    break;
                case 'L':
                    player->SetDirection(Direction::WEST);
                    break;
                }

                http::response<http::string_body> response;

                response.result(http::status::ok);
                response.version(request_.version());
                response.set(http::field::content_type, "application/json");
                response.set(http::field::cache_control, "no-cache");
                response.content_length(response.body().size());

                response.prepare_payload();

                
                return send_(std::move(response));
            }

            Application& application_;
            http::request<Body, http::basic_fields<Allocator>> request_;
            Send send_;
        };

        template <typename Body, typename Allocator, typename Send>
        void HandleGetMaps(http::request<Body, http::basic_fields<Allocator>>&& request, Send&& send, application::Application& application) {
            http::response<http::string_body> response;

            response.result(http::status::ok);
            response.version(request.version());
            response.set(http::field::content_type, "application/json");
            response.keep_alive(request.keep_alive());

            boost::json::array maps_json;
            for (const auto& map : application.GetMaps()) {
                maps_json.emplace_back(boost::json::object{ {"id", static_cast<std::string>(map.GetId())}, {"name", static_cast<std::string>(map.GetName())} });
            }

            response.body() = boost::json::serialize(maps_json);
            response.content_length(response.body().size());

            response.prepare_payload();
            return send(std::move(response));
        }

        template <typename Body, typename Allocator, typename Send>
        void HandleGetMap(http::request<Body, http::basic_fields<Allocator>>&& request, Send&& send, application::Application& application) {
            auto target = request.target();
            std::string target_str = std::string(target);

            std::string expected_prefix = "/api/v1/map/";

            std::string map_id = target_str.substr(expected_prefix.size() + 1);

            const auto map = application.GetMap(map_id);
            if (!map) {
                BadRequestBuilder handler;
                handler.version = request.version();
                handler.status = http::status::not_found;
                handler.code = "mapNotFound";
                handler.message = "Map not found";

                handler.HandleBadRequest(std::move(send));
                return;
            }

            http::response<http::string_body> response;

            response.result(http::status::ok);
            response.version(request.version());
            response.set(http::field::content_type, "application/json");
            response.keep_alive(request.keep_alive());

            response.body() = boost::json::serialize(JsonSerializer::SerializeMap(*map));
            response.content_length(response.body().size());

            response.prepare_payload();
            return send(std::move(response));
        }


        class ApiRequestHandler {
        public:
            ApiRequestHandler(Application& application) : application_(application) {
            }

            template <typename Body, typename Allocator, typename Send>
            void HandleRequest(http::request<Body, http::basic_fields<Allocator>>&& request, Send&& send) {
                ProcessRequest(std::move(request), std::forward<Send>(send));
            }
        private:
            template <typename Body, typename Allocator, typename Send>
            void ProcessRequest(http::request<Body, http::basic_fields<Allocator>>&& request, Send&& send) {
                auto target = request.target();
                std::string target_str = std::string(target);

                std::string base_target = "/api/v1/";

                if (target_str.starts_with(base_target + "game") ) {
                    GameHandler<Body, Allocator, Send> handler(application_, std::move(request), std::move(send));
                    handler.Run();
                }
                else if (target_str == base_target + "maps") {
                    HandleGetMaps(std::move(request), std::move(send), application_);
                }
                else if (target_str.starts_with(base_target + "maps/")) {
                    HandleGetMap(std::move(request), std::move(send), application_);
                }
                else {
                    BadRequestBuilder handler;
                    handler.version = request.version();
                    handler.status = http::status::bad_request;
                    handler.code = "badRequest";
                    handler.message = "Bad request";

                    handler.HandleBadRequest(std::move(send));
                }
            }

            
            Application& application_;
        };      
    } // namespace api_handler
} // namespace http_handler