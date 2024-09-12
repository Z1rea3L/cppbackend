cmake_minimum_required(VERSION 3.13)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(SRCDIR src)

set(EXECUTE game_server)

set(SRC 
	${SRCDIR}/main.cpp
  ${SRCDIR}/logger.cpp
	${SRCDIR}/boost_json.cpp
	${SRCDIR}/json_loader.cpp
#	${SRCDIR}/tag_invokers.cpp
  ${SRCDIR}/util.cpp
  ${SRCDIR}/application.cpp
  ${SRCDIR}/response_maker.cpp
  ${SRCDIR}/api_handler.cpp
  ${SRCDIR}/content_type.cpp
	#${SRCDIR}/request_handler.cpp
)

set(HEADER 
  ${SRCDIR}/sdk.h
  ${SRCDIR}/logger.h
  ${SRCDIR}/json_loader.h
  ${SRCDIR}/http_server.h
  ${SRCDIR}/tagged.h
#  ${SRCDIR}/tag_invokers.h
  ${SRCDIR}/application.h
  ${SRCDIR}/request_handler.h
  ${SRCDIR}/content_type.h
  ${SRCDIR}/util.h
  ${SRCDIR}/response_maker.h

)




find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



add_executable(${EXECUTE} ${SRC} ${HEADER})
target_include_directories(${EXECUTE} PRIVATE CONAN_PKG::boost)
target_link_libraries(${EXECUTE} PRIVATE Threads::Threads CONAN_PKG::boost)
add_subdirectory(${SRCDIR}/http_server)
add_subdirectory(${SRCDIR}/model)
