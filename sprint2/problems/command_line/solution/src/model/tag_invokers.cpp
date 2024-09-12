#include "tag_invokers.h"

namespace model {
using namespace gm;
namespace {

template <typename T>
std::enable_if_t<std::is_integral_v<T>, T>
extruct(const value& jv, const string_view& key) {
  return static_cast<T>(jv.as_object().at(key).as_int64());
}

template <typename T>
std::enable_if_t<std::is_same_v<T, std::string>, std::string>
extruct(const value& jv, const string_view& key) {
  return static_cast<std::string>(jv.as_object().at(key).as_string());
}

}  // namespace

void tag_invoke(value_from_tag, value& jv, Building const& building) {
  jv = {{MapKey::pos_X, building.GetBounds().position.x},
        {MapKey::pos_Y, building.GetBounds().position.y},
        {MapKey::width, building.GetBounds().size.width},
        {MapKey::height, building.GetBounds().size.height}};
}

void tag_invoke(value_from_tag, value& jv, Office const& office) {
  jv = {
      {MapKey::id, *office.GetId()},
      {MapKey::pos_X, office.GetPosition().x},
      {MapKey::pos_Y, office.GetPosition().y},
      {MapKey::offset_X, office.GetOffset().dx},
      {MapKey::offset_Y, office.GetOffset().dy}};
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

  const Coord& x = extruct<Coord>(jv, MapKey::pos_X);
  const Coord& y = extruct<Coord>(jv, MapKey::pos_Y);
  const Dimension& w = extruct<Dimension>(jv, MapKey::width);
  const Dimension& h = extruct<Dimension>(jv, MapKey::height);

  if (w == 0 || h == 0) {
    throw std::logic_error("Wrong building size");
  }

  return Building({{x, y}, {w, h}});
}

Office tag_invoke(value_to_tag<Office>, value const& jv) {
  auto& office = jv.as_object();

  Office::Id id(extruct<std::string>(jv, MapKey::id));

  const Coord& x = extruct<Coord>(jv, MapKey::pos_X);
  const Coord& y = extruct<Coord>(jv, MapKey::pos_Y);
  const Dimension& ofX = extruct<Dimension>(jv, MapKey::offset_X);
  const Dimension& ofY = extruct<Dimension>(jv, MapKey::offset_Y);

  Point2i position{x, y};
  Offset offset{ofX, ofY};

  return {id, position, offset};
}

Road tag_invoke(value_to_tag<Road>, value const& jv) {
  Point2i start{};
  Coord finish{};

  auto& road = jv.as_object();
  if (road.size() != 3) {
    throw std::logic_error("wrong argument in road value");
  }

  bool vertical = true;
  for (auto i = road.cbegin(); i != road.cend(); ++i) {
    if (i->key() == MapKey::start_X) {
      start.x = static_cast<Coord>(i->value().as_int64());
    } else if (i->key() == MapKey::start_Y) {
      start.y = static_cast<Coord>(i->value().as_int64());
    } else if (i->key() == MapKey::end_X) {
      vertical = false;
      finish = static_cast<Coord>(i->value().as_int64());
    } else if (i->key() == MapKey::end_Y) {
      vertical = true;
      finish = static_cast<Coord>(i->value().as_int64());
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
    } else {
      // throw std::logic_error(
      //     std::string("Found unknon key").append(i->key_c_str()));
    }
  }
  return map;
}

}  // namespace model
