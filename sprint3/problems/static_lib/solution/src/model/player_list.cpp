#include "player_list.h"

namespace model {

PlayerList::PlayerList() {
}

PlayerList::~PlayerList() {
}

Player* PlayerList::FindPlayer(const Token& token) {
  if (m_players.contains(token)) {
    return &m_players.at(token);
  }
  return nullptr;
}

Player& PlayerList::CreatePlayer(const std::string& name) {
  Token token = GenerateToken();
  auto pr = m_players.emplace(token, Player(token, name));
  if (pr.second) {
    return m_players.at(token);
  }
  throw std::runtime_error("Fail to add or create Player");
}

const PlayerList::Container& PlayerList::GetContainer() const{
  return m_players;
}


}  // namespace model