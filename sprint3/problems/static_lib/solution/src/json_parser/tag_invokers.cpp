#include "tag_invokers.h"

namespace model {
namespace {

template <typename T>
T extruct(const value& jv, const string_view& key);

template <>
int extruct<int>(const value& jv, const string_view& key) {
  return static_cast<int>(jv.as_object().at(key).as_int64());
}

template <>
double extruct<double>(const value& jv, const string_view& key) {
  return jv.as_object().at(key).as_double();
}

template <>
std::string extruct<std::string>(const value& jv, const string_view& key) {
  return static_cast<std::string>(jv.as_object().at(key).as_string());
}

}  // namespace

void tag_invoke(value_from_tag, value& jv, Building const& building) {
  jv = {{MapKey::pos_X, building.GetBounds().left},
        {MapKey::pos_Y, building.GetBounds().top},
        {MapKey::width, building.GetBounds().width},
        {MapKey::height, building.GetBounds().height}};
}

void tag_invoke(value_from_tag, value& jv, Office const& office) {
  jv = {
      {MapKey::id, *office.GetId()},
      {MapKey::pos_X, office.GetPosition().x},
      {MapKey::pos_Y, office.GetPosition().y},
      {MapKey::offset_X, office.GetOffset().x},
      {MapKey::offset_Y, office.GetOffset().y}};
}

void tag_invoke(value_from_tag, value& jv, Road const& road) {
  object obj;
  obj[MapKey::start_X] = road.GetStart().x;
  obj[MapKey::start_Y] = road.GetStart().y;
  if (road.IsHorizontal()) {
    obj[MapKey::end_X] = road.GetEnd().x;
  } else {
    obj[MapKey::end_Y] = road.GetEnd().y;
  }

  jv = value_from(obj);
}

void tag_invoke(value_from_tag, value& jv, Map const& map) {
  object obj;
  obj[MapKey::id] = *map.GetId();
  obj[MapKey::name] = map.GetName();

  // loot types parse
  if (!map.GetLootTypes().empty()) {
    array arr;
    for (auto& i : map.GetLootTypes()) {
      arr.push_back(value_from(i));
    }
    obj[MapKey::lootTypes] = value_from(arr);
  }

  // road parse
  if (!map.GetRoads().empty()) {
    array arr;
    for (auto& i : map.GetRoads()) {
      arr.push_back(value_from(i));
    }

    obj[MapKey::roads] = value_from(arr);
  }

  // building parse
  if (!map.GetBuildings().empty()) {
    array arr;
    for (auto& i : map.GetBuildings()) {
      arr.push_back(value_from(i));
    }
    obj[MapKey::buildings] = value_from(arr);
  }

  // office parse
  if (!map.GetOffices().empty()) {
    array arr;
    for (auto& office : map.GetOffices()) {
      arr.push_back(value_from(office));
    }
    obj[MapKey::offices] = value_from(arr);
  }
  jv = obj;
}

void tag_invoke(value_from_tag, value& jv, LootType const& lootType) {
  object obj;
  if (lootType.name) {
    obj["name"] = *lootType.name;
  }

  if (lootType.file) {
    obj["file"] = *lootType.file;
  }

  if (lootType.type) {
    obj["type"] = *lootType.type;
  }

  if (lootType.rotation) {
    obj["rotation"] = *lootType.rotation;
  }

  if (lootType.color) {
    obj["color"] = *lootType.color;
  }

  if (lootType.scale) {
    obj["scale"] = *lootType.scale;
  }

  jv = std::move(obj);
}

void tag_invoke(value_from_tag, value& jv, Dog const& dog) {
  jv = {
      {"pos", {dog.GetPosition().x, dog.GetPosition().y}},
      {"speed", {dog.GetSpeed().x, dog.GetSpeed().y}},
      {"dir", dog.GetDir()}};
}

// ------------------------------------

Building tag_invoke(value_to_tag<Building>, value const& jv) {
  if (jv.as_object().size() != 4) {
    throw std::logic_error("wrong building val");
  }

  RectInt rect;

  rect.left = extruct<int>(jv, MapKey::pos_X);
  rect.top = extruct<int>(jv, MapKey::pos_Y);
  rect.width = extruct<int>(jv, MapKey::width);
  rect.height = extruct<int>(jv, MapKey::height);

  if (rect.width == 0 || rect.height == 0) {
    throw std::logic_error("Wrong building size");
  }

  return Building(rect);
}

Office tag_invoke(value_to_tag<Office>, value const& jv) {
  auto& office = jv.as_object();

  Office::Id id(extruct<std::string>(jv, MapKey::id));

  Point2i position;
  Point2i offset;

  position.x = extruct<int>(jv, MapKey::pos_X);
  position.y = extruct<int>(jv, MapKey::pos_Y);
  offset.x = extruct<int>(jv, MapKey::offset_X);
  offset.y = extruct<int>(jv, MapKey::offset_Y);

  return {id, position, offset};
}

Road tag_invoke(value_to_tag<Road>, value const& jv) {
  Point2i start;
  int finish;

  auto& road = jv.as_object();
  if (road.size() != 3) {
    throw std::logic_error("wrong argument in road value");
  }

  bool vertical = true;
  for (auto i = road.cbegin(); i != road.cend(); ++i) {
    if (i->key() == MapKey::start_X) {
      start.x = static_cast<int>(i->value().as_int64());
    } else if (i->key() == MapKey::start_Y) {
      start.y = static_cast<int>(i->value().as_int64());
    } else if (i->key() == MapKey::end_X) {
      vertical = false;
      finish = static_cast<int>(i->value().as_int64());
    } else if (i->key() == MapKey::end_Y) {
      vertical = true;
      finish = static_cast<int>(i->value().as_int64());
    } else {
      throw std::logic_error("Unknon key in road json");
    }
  }

  if (vertical) {
    return {Road::VERTICAL, start, finish};
  }
  return {Road::HORIZONTAL, start, finish};
}

Map tag_invoke(value_to_tag<Map>, value const& jv) {
  auto& m = jv.as_object();
  Map map(
      Map::Id(extruct<std::string>(jv, MapKey::id)),
      extruct<std::string>(jv, MapKey::name));

  if (m.contains("dogSpeed")) {
    map.SetDogSpeed(m.at("dogSpeed").as_double());
  }

  for (auto i = m.cbegin(); i != m.cend(); ++i) {
    if (i->key() == MapKey::id || i->key() == MapKey::name) {
      // pass
    } else if (i->key() == MapKey::roads) {
      for (auto r : i->value().as_array()) {
        map.AddRoad(value_to<Road>(r));
      }
    } else if (i->key() == MapKey::buildings) {
      for (auto b : i->value().as_array()) {
        map.AddBuilding(value_to<Building>(b));
      }
    } else if (i->key() == MapKey::offices) {
      for (auto office : i->value().as_array()) {
        map.AddOffice(value_to<Office>(office));
      }
    } else if (i->key() == MapKey::lootTypes) {
      for (auto lootType : i->value().as_array()) {
        map.AddLootType(value_to<LootType>(lootType));
      }
    } else {
      // throw std::logic_error(
      //     std::string("Found unknon key").append(i->key_c_str()));
    }
  }
  return map;
}

LootType tag_invoke(value_to_tag<LootType>, value const& jv) {
  LootType ret;
  const object& obj = jv.as_object();

  if (obj.contains(MapKey::name)) {
    ret.name = obj.at(MapKey::name).as_string();
  }
  if (obj.contains(MapKey::file)) {
    ret.file = obj.at(MapKey::file).as_string();
  }
  if (obj.contains(MapKey::type)) {
    ret.type = obj.at(MapKey::type).as_string();
  }
  if (obj.contains(MapKey::rotation)) {
    ret.rotation = obj.at(MapKey::rotation).as_int64();
  }
  if (obj.contains(MapKey::color)) {
    ret.color = obj.at(MapKey::color).as_string();
  }
  if (obj.contains(MapKey::scale)) {
    ret.scale = obj.at(MapKey::scale).as_double();
  }

  return ret;
}

Game tag_invoke(value_to_tag<Game>, value const& jv) {
  model::Game game;

  if (jv.as_object().contains(MapKey::defaultDogSpeed)) {
    game.SetDefaultSpeed(jv.as_object().at(MapKey::defaultDogSpeed).as_double());
  } else {
    game.SetDefaultSpeed(1.0);
  }

  if (jv.as_object().contains(MapKey::lootGeneratorConfig)) {
    auto& loot = jv.as_object().at(MapKey::lootGeneratorConfig);
    game.LootGeneratorConfig(loot.at(MapKey::period).as_double(), loot.at(MapKey::probability).as_double());
  } else {
    assert(!"no loot config");
  }

  auto maps = jv.as_object().at(MapKey::maps).as_array();

  for (auto m = maps.cbegin(); m != maps.cend(); ++m) {
    model::Map ext_map = boost::json::value_to<model::Map>(*m);
    game.AddMap(ext_map);
  }

  return game;
}

}  // namespace model
