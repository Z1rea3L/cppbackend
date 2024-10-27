#pragma once

#include <boost/json.hpp>

#include <cmath>
#include <deque>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "collision_detector.h"
#include "loot_generator.h"
#include "tagged.h"

namespace model {

namespace json = boost::json;

using namespace std::literals;

int GetRandom(int from, int to);

using Dimension = int;
using Coord     = Dimension;

struct Point {
    Coord x, y;
    std::string ToString() {
        std::ostringstream oss;
        oss << "[ " << x << ", " << y << " ]";
        return oss.str();
    }
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

enum Direction {
    NORTH, // U
    SOUTH, // D
    WEST,  // L
    EAST   // R
};

std::string DirToStr(Direction dir);
bool StrToDir(std::string str, Direction& dir);

struct Position {
    double x = 0;
    double y = 0;
    std::string ToString() {
        std::ostringstream oss;
        oss << "[ " << x << ", " << y << " ]";
        return oss.str();
    }
};

struct Speed {
    double sx = 0;
    double sy = 0;
    std::string ToString() {
        std::ostringstream oss;
        oss << "[ " << sx << ", " << sy << " ]";
        return oss.str();
    }
};

struct Movement {
    double distanse;
    bool   stop;
    std::string ToString() {
        std::ostringstream oss;
        oss << "[ " << distanse << ", " << std::boolalpha << stop << " ]";
        return oss.str();
    }
};

bool MoveComparator(const Movement& first, const Movement& second);

struct LootType {
    constexpr static std::string_view TYPE_DEFAULT = "obj"sv;
    constexpr static double SCALE_DEFAULT    = 1.0;
    constexpr static int ROTATION_DEFAULT = -1;
    constexpr static std::string_view COLOR_DEFAULT = ""sv;

    LootType() = default;
    LootType(std::string name,
             std::string file,
             std::string_view type = TYPE_DEFAULT,
             double scale = SCALE_DEFAULT,
             int rotation = ROTATION_DEFAULT,
             std::string_view color = COLOR_DEFAULT,
             unsigned value = 0)
        : name_(name)
        , file_(file)
        , type_(type)
        , scale_(scale)
        , rotation_(rotation)
        , color_(color)
        , value_(value) {
    }

    std::string name_;
    std::string file_;
    std::string type_;
    double scale_;
    int rotation_;
    std::string color_;
    unsigned value_;

    json::object ToJson() const;
    static LootType FromJson(json::object json_loot_type);
};

struct LostObject {
    static unsigned CURR_ID;

    LostObject() = default;
    LostObject(unsigned type, const Position& position)
        : id_(CURR_ID++)
        , type_(type)
        , position_(position) {
    }

    unsigned id_;
    unsigned type_;
    Position position_;

    json::object ToJson() const;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

    json::object ToJson() const;
    static Road FromJson(json::object json_road);

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

    json::object ToJson() const;
    static Building FromJson(json::object json_building);

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

    json::object ToJson() const;
    static Office FromJson(json::object json_office);

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;
    using LootTypes = std::vector<LootType>;

    Map(Id id, std::string name, double dog_speed, unsigned bag_capacity) noexcept
        : id_(std::move(id))
        , name_(std::move(name))
        , dog_speed_(dog_speed)
        , bag_capacity_(bag_capacity) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    double GetDogSpeed() const noexcept {
        return dog_speed_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

    std::string Serialize() const;
    static Map FromJson(json::object json_map, double default_dog_speed, unsigned default_bag_capacity);

    size_t GetLootsCount() const noexcept {
        return loot_types_.size();
    }

    const LootTypes& GetLootTypes() const noexcept {
        return loot_types_;
    }

    void AddLootType(const LootType& loot_type) {
        loot_types_.emplace_back(loot_type);
    }

    unsigned GetBagCapacity() const noexcept { return bag_capacity_; }

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;
    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    double dog_speed_;
    LootTypes loot_types_;
    unsigned bag_capacity_;
};

struct BagItem {
    size_t id_;
    unsigned type_;
};

using Bag = std::vector<BagItem>;

constexpr static double LOOT_WIDTHS = 0.0;
constexpr static double OFFICE_WIDTHS = 0.5;
constexpr static double DOG_WIDTHS = 0.6;

class GameProvider : public collision_detector::ItemGathererProvider {
public:
    size_t ItemsCount() const override { return items_.size(); }
    collision_detector::Item GetItem(size_t idx) const override { return items_.at(idx); }
    size_t GatherersCount() const override { return gatherers_.size(); }
    collision_detector::Gatherer GetGatherer(size_t idx) const override { return gatherers_.at(idx); }

    void PushItem(collision_detector::Item item) { items_.push_back(item); }
    void PushGatherer(collision_detector::Gatherer gatherer) { gatherers_.push_back(gatherer); }

private:
    std::vector<collision_detector::Item> items_;
    std::vector<collision_detector::Gatherer> gatherers_;
};

class Dog {
    const double ROAD_WIDTH = 0.8;

public:
    Dog(std::string name, uint32_t id) : name_(name), id_(id) { }

    const std::string GetName() const { return name_;}
    const uint32_t GetId() const { return id_;}
    Position GetPosition() const { return pos_;}
    Position GetStartPos() const { return start_pos_;}
    Speed GetSpeed() const { return speed_;}
    std::string GetDir() const { return DirToStr(dir_); }

    void SetPosition(Position pos) { pos_ = pos; }
    void SetSpeed(double speed, std::string move) {
        if ( move.empty() ) { Stop(); return; }
        if ( move == "U" ) { dir_ = NORTH; speed_ = { 0,-speed }; return; }
        if ( move == "D" ) { dir_ = SOUTH; speed_ = { 0, speed }; return; }
        if ( move == "R" ) { dir_ = EAST;  speed_ = { speed, 0 }; return; }
        if ( move == "L" ) { dir_ = WEST;  speed_ = {-speed, 0 }; return; }
    }

    void Move(uint32_t time_delta, const Map::Roads& roads);

    const Bag& GetBag() const { return bag_; }
    void SetBagCapacity(unsigned bag_capacity) { bag_capacity_ = bag_capacity; }
    bool PushIntoBag(BagItem bag_item, unsigned value);
    void EmptyBag();
    unsigned GetScore() const noexcept { return score_; }

private:
    Movement MoveOnRoad(double time, const Road& road);
    void Stop() { speed_ = { 0, 0 }; }
    bool IsStopped() { return speed_.sx == 0.0 && speed_.sy == 0.0; }

private:
    std::string name_;
    uint32_t id_;
    Position pos_{0, 0};
    Position start_pos_{0, 0};
    Speed speed_{0, 0};
    Direction dir_{NORTH};
    Bag bag_;
    unsigned bag_capacity_ = 0;
    unsigned score_ = 0;
    unsigned value_ = 0;
};

class GameSession {
public:
    using LostObjects = std::vector<LostObject>;

    explicit GameSession(const Map* map) : map_(map) { }
    Dog* AddDog(std::string name, uint32_t id) { 
        dogs_.emplace_back(name, id);
        return &dogs_.back();
    }

    const Map* GetMap() const { return map_; }
    const std::deque<Dog>& GetDogs() const { return dogs_; }
    size_t GetDogsCount()  const noexcept { return dogs_.size(); }

    void Tick(uint32_t time_delta, unsigned lost_count);

    size_t GetLostsCount() const noexcept { return lost_objects_.size(); }
    const LostObjects& GetLostObjects() const noexcept { return lost_objects_; }
    
    void AddLostObject(const LostObject& lost_object) {
        lost_objects_.emplace_back(lost_object);
    }

private:
    const Map* map_;
    std::deque<Dog> dogs_;
    LostObjects lost_objects_;
};

class Game {
public:
    using Maps = std::vector<Map>;
    using Sessions = std::vector<GameSession>;
    constexpr static size_t SESSION_MAX = 16;

    Game(unsigned period, double probability) : loot_gen_(std::chrono::milliseconds{period}, probability) {
        sessions_.reserve(SESSION_MAX);
    }

    void AddMap(Map map);

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

    GameSession* AddSession(const Map* map);

    const Sessions& GetSessions() const noexcept {
        return sessions_;
    }

    GameSession* FindSession(const Map::Id& id) noexcept {
        if (auto it = map_id_to_session_.find(id); it != map_id_to_session_.end()) {
            return &sessions_.at(it->second);
        }
        return nullptr;
    }

    void Tick(uint32_t time_delta) {
        for (auto& session : sessions_) {
            session.Tick(time_delta, loot_gen_.Generate(std::chrono::milliseconds{time_delta}, session.GetLostsCount(), session.GetDogsCount()));
        }
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

    Maps maps_;
    MapIdToIndex map_id_to_index_;
    Sessions sessions_;
    MapIdToIndex map_id_to_session_;
    loot_gen::LootGenerator loot_gen_;
};
}//namespace model
