
#set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_MODEL 
  player.cpp  
  player_list.cpp  
  tag_invokers.cpp
  dog.cpp
  game_session.cpp
  game.cpp
  model_map.cpp
  token_generator.cpp
)
message(${SRCDIR})

set(HEADER_MODEL 
  dog.h
  game_session.h
  game.h
  model_map.h
  token_generator.h
  geometry.h
  player.h
  player_list.h
  tag_invokers.h
  token_generator.h
)

message(${SRC})

target_sources(${EXECUTE} PUBLIC ${SRC_MODEL} ${HEADER_MODEL})