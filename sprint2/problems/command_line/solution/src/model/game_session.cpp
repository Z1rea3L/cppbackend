#include "game_session.h"

#include "../logger.h"
namespace model {
GameSession::GameSession(const Map& map) : m_map(map) {
}

GameSession::~GameSession() {
}

const Map& GameSession::GetMap() const noexcept {
  return m_map;
}

std::shared_ptr<Dog> GameSession::GetDog(const Token& token) {
  for (auto& i : m_dogs) {
    auto ptr = i;
    if (ptr->GetToken() == token) {
      return ptr;
    }
  }
  return nullptr;
}

void GameSession::AddDog(std::shared_ptr<Dog> dog) {
  dog->SetPosition(m_map.GetStartDogPoint());
  dog->SetDefaultSpeed(m_map.GetDogSpeed());
  m_dogs.emplace_back(dog);
}

void GameSession::Update(const double& delta) {
  for (auto it = m_dogs.begin(); it != m_dogs.end(); ++it) {
    auto dog = *it;
    assert(dog != nullptr);
    auto dir = dog->GetDir();

    auto pos = dog->GetPosition();
    auto speed = dog->GetSpeed();

    auto posNew = pos + (speed * delta);

    double border_up = pos.y;
    double border_down = pos.y;
    double border_left = pos.x;
    double border_right = pos.x;

    bool check_if_contain = false;
    for( auto road = m_map.GetRoads().cbegin(); road != m_map.GetRoads().cend(); ++road) {
      if(road->Contains(pos)) {
        check_if_contain = true; 
        if(border_up > road->GetMinY()) {
          border_up = road->GetMinY();
        }

        if(border_down < road->GetMaxY()) {
          border_down = road->GetMaxY();
        }

        if(border_left > road->GetMinX()) {
          border_left = road->GetMinX();
        }

        if(border_right < road->GetMaxX()) {
          border_right = road->GetMaxX();
        }
      }
    }

    assert(check_if_contain);

    if (posNew.y < border_up) {
      posNew.y = border_up;
      dog->SetSpeed({0.0, 0.0});
    } else if (posNew.y > border_down) {
      posNew.y = border_down;
      dog->SetSpeed({0.0, 0.0});
    } else if (posNew.x < border_left) {
      posNew.x = border_left;
      dog->SetSpeed({0.0, 0.0});
    } else if (posNew.x > border_right) {
      posNew.x = border_right;
      dog->SetSpeed({0.0, 0.0});
    }

    dog->SetPosition(posNew);
  }
}

}  // namespace model
