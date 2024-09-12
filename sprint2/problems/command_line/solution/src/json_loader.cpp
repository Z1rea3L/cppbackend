#include "json_loader.h"

//#define BOOST_JSON_STANDALONE
#include <boost/json.hpp>
#include <fstream>
#include <string>

using namespace std::literals;

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
  // Загрузить содержимое файла json_path, например, в виде строки
  // Распарсить строку как JSON, используя boost::json::parse
  // Загрузить модель игры из файла

  // open file
  std::ifstream ifs(json_path);
  if (!ifs.is_open()) {
    throw std::runtime_error(("Fail to open "s).append(json_path));
  }

  // get data from file
  const std::string data((std::istreambuf_iterator<char>(ifs)),
                         (std::istreambuf_iterator<char>()));

  ifs.close();

  boost::json::error_code ec;

  // parse
  auto doc = boost::json::parse(data, ec);
  if (ec) {
    throw std::runtime_error(("Fail to json parse "s).append(ec.message()));
  }

  model::Game game;

  if (doc.as_object().contains("defaultDogSpeed")) {
    game.SetDefaultSpeed(doc.as_object().at("defaultDogSpeed").as_double());
  } else {
    game.SetDefaultSpeed(1.0);
  }

  auto maps = doc.as_object().at("maps").as_array();

  for (auto m = maps.cbegin(); m != maps.end(); ++m) {
    model::Map ext_map = boost::json::value_to<model::Map>(*m);
    // if (ext_map.GetDogSpeed() == 0.0) {
    //   ext_map.SetDogSpeed(game.GetDefaultSpeed());
    // }
    game.AddMap(ext_map);
  }

  return game;
}

}  // namespace json_loader
