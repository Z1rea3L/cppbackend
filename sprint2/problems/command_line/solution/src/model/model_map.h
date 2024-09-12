#ifndef __MODEL_MAP_H__
#define __MODEL_MAP_H__

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../tagged.h"
#include "geometry.h"

namespace model {

using namespace gm;

using Dimension = int;
using Coord = Dimension;

struct Size {
  Dimension width, height;
};

struct Rectangle {
  Point2i position;
  Size size;
};

struct Offset {
  Dimension dx, dy;
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

  Road(HorizontalTag, Point2i start, Coord end_x) noexcept 
    : start_{start},
    end_{end_x, start.y},
    m_min_X( std::min(start.x, end_x) - Road::HALF_WIDTH),
    m_max_X( std::max(start.x, end_x) + Road::HALF_WIDTH),
    m_min_Y( start.y - Road::HALF_WIDTH),
    m_max_Y( start.y + Road::HALF_WIDTH)
    {}

  Road(VerticalTag, Point2i start, Coord end_y) noexcept 
    : start_{start},
    end_{start.x, end_y},
    m_min_X( start.x - Road::HALF_WIDTH),
    m_max_X( start.x + Road::HALF_WIDTH),
    m_min_Y( std::min(start.y, end_y) - Road::HALF_WIDTH),
    m_max_Y( std::max(start.y, end_y) + Road::HALF_WIDTH)
    {}

  bool IsHorizontal() const noexcept { return start_.y == end_.y; }

  bool IsVertical() const noexcept { return start_.x == end_.x; }

  Point2i GetStart() const noexcept { return start_; }

  Point2i GetEnd() const noexcept { return end_; }

  bool Contains(const Point2d& point) const;

  const double& GetMinX() const {return m_min_X;}
  const double& GetMaxX() const {return m_max_X;}
  const double& GetMinY() const {return m_min_Y;}
  const double& GetMaxY() const {return m_max_Y;}

  static constexpr double HALF_WIDTH = 0.4;

 private:
  Point2i start_;
  Point2i end_;

  double m_min_X;
  double m_max_X;
  double m_min_Y;
  double m_max_Y;
};

class Building {
 public:
  explicit Building(Rectangle bounds) noexcept : bounds_{bounds} {}

  const Rectangle& GetBounds() const noexcept { return bounds_; }

 private:
  Rectangle bounds_;
};

class Office {
 public:
  using Id = util::Tagged<std::string, Office>;

  Office(Id id, Point2i position, Offset offset) noexcept : id_{std::move(id)}, position_{position}, offset_{offset} {}

  const Id& GetId() const noexcept { return id_; }

  Point2i GetPosition() const noexcept { return position_; }

  Offset GetOffset() const noexcept { return offset_; }

 private:
  Id id_;
  Point2i position_;
  Offset offset_;
};

class Map {
 public:
  using Id = util::Tagged<std::string, Map>;
  using Roads = std::vector<Road>;
  using RoadsV = std::vector<Road>;
  using RoadsH = std::vector<Road>;
  using Buildings = std::vector<Building>;
  using Offices = std::vector<Office>;

  Map(Id id, std::string name) noexcept : id_(std::move(id)), name_(std::move(name)), m_dog_speed(0.0), m_random_spawn(false) {}

  const Id& GetId() const noexcept { return id_; }

  const std::string& GetName() const noexcept { return name_; }

  const Buildings& GetBuildings() const noexcept { return buildings_; }

  const Roads& GetRoads() const noexcept { return roads_; }

  const Offices& GetOffices() const noexcept { return offices_; }

  void AddRoad(const Road& road) { 
    roads_.emplace_back(road); 
  }

  void AddBuilding(const Building& building) { buildings_.emplace_back(building); }

  void AddOffice(Office office);

  Point2d GetStartDogPoint() const;

  void SetDogSpeed(double speed);

  double GetDogSpeed() const noexcept;

  void EnableRandomStartPoint(const bool& enable = true);

 private:
  Point2d GetRandPoint() const;
  using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

  Id id_;
  std::string name_;
  Roads roads_;
  Buildings buildings_;
  RoadsV m_vroads;
  RoadsH m_hroads;

  OfficeIdToIndex warehouse_id_to_index_;
  Offices offices_;

  double m_dog_speed;
  bool m_random_spawn;
};

}  // namespace model

#endif  // __MODEL_MAP_H__