#pragma once

#include <boost/json.hpp>

#include <filesystem>
#include <fstream>

#include <stdexcept>
#include <string>

#include "model.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& config_file);

void LoadRoads(const boost::json::array& roads, model::Map& map);
void LoadBuildings(const boost::json::array& buildings, model::Map& map);
void LoadOffices(const boost::json::array& offices, model::Map& map);

}  // namespace json_loader
