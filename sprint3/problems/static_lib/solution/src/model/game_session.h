#ifndef __MODEL_GSES_H__
#define __MODEL_GSES_H__

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <list>

#include "../tagged.h"
#include "dog.h"
#include "model_map.h"
#include "loot_generator.h"
#include "loot.h"

namespace model {

class GameSession {
 public:
  using LootContainer = std::list<Loot>;

  explicit GameSession(const Map& map, LootGenerator gen);
  ~GameSession();

  const Map& GetMap() const noexcept;

  std::shared_ptr<Dog> GetDog(const Token& token);

  void AddDog(std::shared_ptr<Dog> dog);

  void Update(const double& delta);

  void SetDogRandomSpawn(bool enable = true);

  const LootContainer GetLoots() const noexcept { return m_loots;}

 private:

  void GenerateLoot(const double& delta);

  void DogsUpdate(const double& delta);

  const Map& m_map;
  std::vector<std::shared_ptr<Dog>> m_dogs;
  bool m_random_spawn;

  LootGenerator m_loot_gen;

  LootContainer m_loots;
};

}  // namespace model

#endif  // __MODEL_GSES_H__