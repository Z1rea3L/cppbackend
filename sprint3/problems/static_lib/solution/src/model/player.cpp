#include "player.h"

namespace model {

int Player::ID_COUNTER = 0;

Player::Player(const Token& token, const std::string& name) : m_id(ID_COUNTER++), m_token(token), m_name(name), m_dog(std::make_shared<Dog>(token)) {
}

int Player::GetId() const noexcept {
  return m_id;
}

const std::string& Player::GetName() const noexcept {
  return m_name;
}

std::shared_ptr<Dog> Player::GetDog() {
  return m_dog;
}

const std::shared_ptr<Dog> Player::GetDog() const {
  return m_dog;
}

const Token& Player::GetToken() const {
  return m_token;
}

std::shared_ptr<GameSession> Player::GetSession() {
  return m_gs;
}

const std::shared_ptr<GameSession> Player::GetSession() const {
  return m_gs;
}

void Player::SetSession(std::shared_ptr<GameSession> sess) {
  m_gs = sess;
}

}  // namespace model
