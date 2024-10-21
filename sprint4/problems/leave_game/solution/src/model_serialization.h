#ifndef __STATE_SERIALIZE_H__
#define __STATE_SERIALIZE_H__

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
// #include <boost/serialization/optional.hpp>
#include <boost/serialization/utility.hpp>

#include "model.h"

#include "ser/point_ser.h"
#include "ser/loot_type_ser.h"
#include "ser/game_object_ser.h"
#include "ser/loot_ser.h"
#include "ser/dog_ser.h"
#include "ser/game_session_ser.h"

namespace model {



}  // namespace model

// BOOST_SERIALIZATION_SPLIT_FREE(model::GameObject);

// class SessSer {
//   SessSer(model::Game::SessPtrList& sess) : m_sess(sess) {}
//  public:
//   template<class Archive>
//   void serialize(Archive &ar, const unsigned int version) {
//   }

//   model::Game::SessPtrList& m_sess;
// };

// class GameSer {
//  public:
//   explicit GameSer(model::Game &game, model::PlayerList& plist) : m_game(game) , m_plist(plist) {
//   }

//   template<class Archive>
//   void serialize(Archive &ar, const unsigned int version) {
//      ar & SessSer(const_cast<model::Game::SessPtrList&>(m_game.GetSessionList()));
//      ar & m_plist;
//   }

//  private:

//  model::Game &m_game;
//  model::PlayerList m_plist;

// };

#endif  // __STATE_SERIALIZE_H__