#pragma once

#include "app.h"
#include "model.h"
#include "response.h"

namespace http_handler {

class ApiHandler {
    constexpr static std::string_view API = "/api"sv;
    constexpr static std::string_view MAPS = "/api/v1/maps"sv;
    constexpr static std::string_view JOIN = "/api/v1/game/join"sv;
    constexpr static std::string_view PLAYERS = "/api/v1/game/players"sv;
    constexpr static std::string_view STATE = "/api/v1/game/state"sv;
    constexpr static std::string_view MOVE = "/api/v1/game/player/action"sv;
    constexpr static std::string_view TICK = "/api/v1/game/tick"sv;

    constexpr static std::string_view BEARER = "Bearer "sv;
    constexpr static size_t TOKEN_SIZE = 32;

public:
    explicit ApiHandler(app::Application& app)
        : app_(app) {
    }

    ApiHandler(const ApiHandler&) = delete;
    ApiHandler& operator=(const ApiHandler&) = delete;

    void SetDebugMode(bool debug_mode) { debug_mode_ = debug_mode; }
    bool CanAccept(const std::string& target) { return target.find(API) == 0; }

    StringResponse Response(const StringRequest& req);

private:
    static std::string MethodToString(http::verb verb);

    bool CheckToken(const StringRequest& req, std::string& token);

    //map by ID
    bool IsMapRequest(std::string target) const { return target.find(MAPS) == 0 && target.size() > MAPS.size() + 1; }
    StringResponse MapResponse(const StringRequest& req);
    
    //all maps
    bool IsMapsRequest(std::string target) const { return target == MAPS; }
    StringResponse MapsResponse(const StringRequest& req);

    //join
    bool IsJoinRequest(std::string target) const { return target == JOIN; }
    StringResponse JoinResponse(const StringRequest& req);

    //players
    bool IsPlayersRequest(std::string target) const { return target == PLAYERS; }
    StringResponse PlayersResponse(const StringRequest& req);

    //state
    bool IsStateRequest(std::string target) const { return target == STATE; }
    StringResponse StateResponse(const StringRequest& req);

    //move
    bool IsMoveRequest(std::string target) const { return target == MOVE; }
    StringResponse MoveResponse(const StringRequest& req);

    //tick
    bool IsTickRequest(std::string target) const { return target == TICK; }
    StringResponse TickResponse(const StringRequest& req);

    app::Application& app_;
    bool debug_mode_ = false;
};

}//namespace http_handler
