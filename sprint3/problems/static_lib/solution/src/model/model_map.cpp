#include "model_map.h"

#include <random>

#include "../logger.h"
namespace model {

using namespace std::literals;

void Map::AddOffice(Office office) {
  if (warehouse_id_to_index_.contains(office.GetId())) {
    throw std::invalid_argument("Duplicate warehouse");
  }

  const size_t index = offices_.size();
  Office& o = offices_.emplace_back(std::move(office));
  try {
    warehouse_id_to_index_.emplace(o.GetId(), index);
  } catch (const std::exception&) {
    // Удаляем офис из вектора, если не удалось вставить в unordered_map
    offices_.pop_back();
    throw;
  }
}

void Map::AddLootType(LootType loot) {
  m_loot_types.push_back(loot);
}

Point2d Map::GetRandPoint(bool enable) const {
  if (roads_.size() == 0) {
    throw std::logic_error("Must be one or mode roads");
  }

  if (!enable) {
    return roads_.at(0).GetStart();
  }

  std::random_device rd;
  std::mt19937 gen{rd()};
  int num_r = 0;
  if (roads_.size() > 1) {
    std::uniform_int_distribution<int> dist(0, roads_.size() - 1);
    num_r = dist(gen);
  }

  Road road = roads_.at(num_r);

  Point2i start = road.GetStart();
  Point2i end = road.GetEnd();

  Point2d ret{static_cast<double>(start.x), static_cast<double>(start.y)};

  // random by wide has no tested
  // double width;
  // {
  //   std::uniform_real_distribution<> dist(-Road::HALF_WIDTH, Road::HALF_WIDTH);
  //   width = dist(gen);
  //   width = ((int)(width * 100 + .5) / 100.0);  // round two digit
  // }

  if (road.IsHorizontal()) {
    std::uniform_real_distribution<> dist(start.x, end.x);
    ret.x = dist(gen);
    // ret.y += width;
  } else {
    std::uniform_real_distribution<> dist(start.y, end.y);
    ret.y = dist(gen);
    // ret.x += width;
  }

  ret.x = ((int)(ret.x * 100 + .5) / 100.0);  // round two digit
  ret.y = ((int)(ret.y * 100 + .5) / 100.0);  // round two digit

  return ret;
}

void Map::SetDogSpeed(double speed) {
  m_dog_speed = speed;
}

double Map::GetDogSpeed() const noexcept {
  return m_dog_speed;
}

}  // namespace model