#pragma once
#ifndef __MODEL__LOOT_H__
#define __MODEL__LOOT_H__

#include <string>

#include "geometry.h"

namespace model {

struct LootType {
  std::optional<std::string> name;   // example -> = "key";
  std::optional<std::string> file;   // example -> = "assets/key.obj";
  std::optional<std::string> type;   // example -> = "obj";
  std::optional<int> rotation;       // example -> = 90;
  std::optional<std::string> color;  // example -> = "#338844";
  std::optional<double> scale;       // example -> = 0.03;
};

class Loot {
 public:
  Loot(const LootType& type,const Point2d& pos, const unsigned& loot_index) : m_loot_type(type), m_position(pos), m_loot_index(loot_index) {}

  // void SetPosition(const Point2d& pos);

  const LootType& GetLootType() const noexcept { return m_loot_type;}
  const Point2d& GetPosition() const noexcept {return m_position;}
  const unsigned& GetLootIndex() const noexcept {return m_loot_index;}


 private:
  const LootType m_loot_type;
  const Point2d m_position;
  const unsigned m_loot_index;
};

}  // namespace model

#endif  // __MODEL__LOOT_H__