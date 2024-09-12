#ifndef __MODEL_GSES_H__
#define __MODEL_GSES_H__

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "../tagged.h"
#include "dog.h"
#include "model_map.h"

namespace model {

class GameSession {
 public:
  explicit GameSession(const Map& map);
  ~GameSession();

  const Map& GetMap() const noexcept;

  std::shared_ptr<Dog> GetDog(const Token& token);

  void AddDog(std::shared_ptr<Dog> dog);

  void Update(const double& delta);

 private:
  const Map& m_map;
  std::vector<std::shared_ptr<Dog>> m_dogs;
};

}  // namespace model

#endif  // __MODEL_GSES_H__