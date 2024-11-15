#include "app.h"

namespace app {

bool Application::GetMap(const std::string& map_id, std::string& res_body) {
    const model::Map* map = game_.FindMap(model::Map::Id(map_id));

    if ( map == nullptr ) {
        return false;
    }
    res_body = map->Serialize();
    
    return true;
}

bool Application::GetMaps(std::string& res_body) {
    json::array json_maps;

    for (const auto& map : game_.GetMaps()) {
        json::object json_map;
        json_map["id"] = *map.GetId();
        json_map["name"] = map.GetName();
        json_maps.push_back(json_map);
    }

    res_body = json::serialize(json_maps);

    return true;
}

bool Application::TryJoin(const std::string& user_name, const std::string& map_id, std::string& res_body) {
    model::Map::Id id(map_id);
    const model::Map* map = game_.FindMap(id);

    if (map == nullptr) {
        return false;
    }

    model::GameSession* session = game_.FindSession(id);

    if (session == nullptr) {
        session = game_.AddSession(map);
    }

    model::Dog* dog = session->AddDog(user_name, dog_id_++);
    dog->SetBagCapacity(map->GetBagCapacity());

    if (randomize_spawn_) {
        auto roads = map->GetRoads();
        int random_road = model::GetRandom(0, roads.size() - 1);
        model::Point start = roads[random_road].GetStart();
        model::Point end = roads[random_road].GetEnd();
        model::Position random_pos;

        if ( roads[random_road].IsVertical() ) {
            random_pos.x = start.x;
            random_pos.y = model::GetRandom(std::min(start.y, end.y), std::max(start.y, end.y));
        } else {
            random_pos.y = start.y;
            random_pos.x = model::GetRandom(std::min(start.x, end.x), std::max(start.x, end.x));
        }
        dog->SetPosition(random_pos);
    }

    std::string token = players_.Add(dog, session);
    Player* player = players_.FindByToken(token);

    if (player == nullptr) {
        std::string err = "Can't get just created player";
        throw std::runtime_error(err);
    }

    json::object result;
    result["authToken"] = token;
    result["playerId"] = player->GetDog()->GetId();
    res_body = json::serialize(result);

    return true;
}

bool Application::GetPlayers(const std::string& token, std::string& res_body) {
    if ( !players_.HasToken(token) ) {
        return false;
    }

    auto players = players_.GetPlayers();
    json::object obj;

    for (auto& player : players) {
        json::object sub_obj;
        sub_obj["name"] = player.GetDog()->GetName();
        obj[std::to_string(player.GetDog()->GetId())] = sub_obj;
    }

    res_body = json::serialize(obj);

    return true;
}

bool Application::GetState(const std::string& token, std::string& res_body) {
    auto player  = players_.FindByToken(token);

    if ( player == nullptr ) {
        return false;
    }

    const model::Map* map = player->GetSession()->GetMap();
    json::object dogs;
    auto players = players_.GetPlayers();
    
    for (auto& player : players) {
        if ( player.GetSession()->GetMap() != map ) {
            continue;
        }
        json::object json_dog;
        json::array json_pos;

        model::Dog* dog = player.GetDog();
        json_pos.push_back(dog->GetPosition().x);
        json_pos.push_back(dog->GetPosition().y);
        json_dog["pos"] = json_pos;

        json::array json_speed;
        json_speed.push_back(dog->GetSpeed().sx);
        json_speed.push_back(dog->GetSpeed().sy);
        json_dog["speed"] = json_speed;
        json_dog["dir"] = dog->GetDir();

        json::array  json_bag;
        for (const auto& bag_item : dog->GetBag()) {
            json::object json_bag_item;
            json_bag_item["id"]   = bag_item.id_;
            json_bag_item["type"] = bag_item.type_;
            json_bag.push_back(json_bag_item);
        }

        json_dog["bag"]   = json_bag;
        json_dog["score"] = dog->GetScore();
        dogs[std::to_string(dog->GetId())] = json_dog;
    }

    const model::GameSession* session = player->GetSession();
    json::object json_losts;

    for (auto& lost : session->GetLostObjects()) {
        json_losts[std::to_string(lost.id_)] = lost.ToJson();
    }

    json::object result;
    result["players"]  = dogs;

    if ( session->GetLostsCount() != 0 ) {
        result["lostObjects"] = json_losts;
    }

    res_body = json::serialize(result);

    return true;
}

bool Application::Move(const std::string& token, const std::string& move, std::string& res_body) {
    Player* player = players_.FindByToken(token);
    if ( player == nullptr ) {
        return false;
    }
    
    double speed = player->GetSession()->GetMap()->GetDogSpeed();
    player->GetDog()->SetSpeed(speed, move);
    res_body = "{}";
    return true;
}

std::string Application::Tick(uint32_t time_delta) {
    game_.Tick(time_delta);
    return "{}"s;
}

}//namespace app
