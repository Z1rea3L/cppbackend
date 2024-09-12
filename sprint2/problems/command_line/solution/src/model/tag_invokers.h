/**
 * @file tag_invokers.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Collection of overload functions for help the boost::json library see
 * @ref take_invoke
 * "https://www.boost.org/doc/libs/1_78_0/libs/json/doc/html/json/dom/conversion.html#json.dom.conversion.tag_invoke_overloads"
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TAG_INVOKERS_H__
#define __TAG_INVOKERS_H__

#include <boost/json.hpp>

#include "game.h"

namespace model {

using namespace boost::json;

void tag_invoke(value_from_tag, value& jv, Building const& building);
void tag_invoke(value_from_tag, value& jv, Office const& office);
void tag_invoke(value_from_tag, value& jv, Road const& road);
void tag_invoke(value_from_tag, value& jv, Map const& map);

void tag_invoke(value_from_tag, value& jv, Dog const& dog);

Building tag_invoke(value_to_tag<Building>, value const& jv);
Office tag_invoke(value_to_tag<Office>, value const& jv);
Road tag_invoke(value_to_tag<Road>, value const& jv);
Map tag_invoke(value_to_tag<Map>, value const& jv);

  /// @brief json object key constant
struct MapKey {
  constexpr static string_view id = "id";
  constexpr static string_view name = "name";
  constexpr static string_view start_X = "x0";
  constexpr static string_view start_Y = "y0";
  constexpr static string_view end_X = "x1";
  constexpr static string_view end_Y = "y1";
  constexpr static string_view pos_X = "x";
  constexpr static string_view pos_Y = "y";
  constexpr static string_view height = "h";
  constexpr static string_view width = "w";
  constexpr static string_view offset_X = "offsetX";
  constexpr static string_view offset_Y = "offsetY";
  constexpr static string_view roads = "roads";
  constexpr static string_view buildings = "buildings";
  constexpr static string_view offices = "offices";
  constexpr static string_view defaultDogSpeed = "defaultDogSpeed";
  constexpr static string_view dogSpeed = "dogSpeed";

};

};  // namespace model

#endif  // __TAG_INVOKERS_H__