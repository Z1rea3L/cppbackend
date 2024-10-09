#ifndef __DOG_H__
#define __DOG_H__

#include "geometry.h"
#include "token_generator.h"

namespace model {

using namespace std::literals;

class Dog {
 public:
  explicit Dog(const Token& token = Token(""s));

  int GetId() const;

  Token GetToken() const;

  bool SetToken(const Token& token);

  const Point2d& GetPosition() const;

  void SetPosition(const Point2d& pos);

  const Point2d& GetSpeed() const;

  void Stop();

  void SetDefaultSpeed(const double& speed);

  const std::string& GetDir() const;

  void SetDir(const std::string& dir);

 private:
  Token m_token;
  int m_id;

  Point2d m_pos;
  Point2d m_speed;
  std::string m_dir;

  double m_default_speed;
  static int ID_COUNTER;
};

}  // namespace model

#endif  // __DOG_H__