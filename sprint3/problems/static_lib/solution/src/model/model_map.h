#ifndef __MODEL_MAP_H__
#define __MODEL_MAP_H__

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../tagged.h"
#include "geometry.h"

#include "office.h"
#include "road.h"
#include "building.h"
#include "loot.h"

namespace model {



class Map {
 public:
  using Id = util::Tagged<std::string, Map>;
  using Roads = std::vector<Road>;
  using Buildings = std::vector<Building>;
  using Offices = std::vector<Office>;
  using LootTypes = std::vector<LootType>;

  Map(Id id, std::string name) noexcept : id_(std::move(id)), name_(std::move(name)), m_dog_speed(0.0) {}

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

  void AddLootType(LootType loot);

  const LootTypes& GetLootTypes() const { return m_loot_types; }

  void SetDogSpeed(double speed);

  double GetDogSpeed() const noexcept;

  Point2d GetRandPoint(bool enable = true) const;
 private:
  using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

  Id id_;
  std::string name_;
  Roads roads_;
  Buildings buildings_;

  OfficeIdToIndex warehouse_id_to_index_;
  Offices offices_;

  double m_dog_speed;

  LootTypes m_loot_types;
};

}  // namespace model

#endif  // __MODEL_MAP_H__