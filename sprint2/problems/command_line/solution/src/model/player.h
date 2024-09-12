#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <memory>

#include "dog.h"
#include "token_generator.h"

namespace model {

class GameSession;

class Player {
 public:
  explicit Player(const Token& token, const std::string &name);

  int GetId() const noexcept;

  const std::string& GetName() const noexcept;

  std::shared_ptr<Dog> GetDog();
  const std::shared_ptr<Dog> GetDog() const ;


  const Token& GetToken() const;

  std::shared_ptr<GameSession> GetSession();
  const std::shared_ptr<GameSession> GetSession() const;
  
  void SetSession(std::shared_ptr<GameSession> sess);

 private:
  int m_id;
  Token m_token;
  std::string m_name;

  std::shared_ptr<GameSession> m_gs;
  std::shared_ptr<Dog> m_dog;

  static int ID_COUNTER;
};

}  // namespace model

#endif  // __PLAYER_H__