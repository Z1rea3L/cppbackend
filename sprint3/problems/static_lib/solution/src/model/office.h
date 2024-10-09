#ifndef __OFFICE_H__
#define __OFFICE_H__

#include <string>

#include "geometry.h"
#include "../tagged.h"

namespace model
{
  class Office {
 public:
  using Id = util::Tagged<std::string, Office>;

  Office(Id id, Point2i position, Point2i offset) noexcept : m_id{std::move(id)}, m_position{position}, m_offset{offset} {}

  const Id& GetId() const noexcept { return m_id; }

  Point2i GetPosition() const noexcept { return m_position; }

  Point2i GetOffset() const noexcept { return m_offset; }

 private:
  Id m_id;
  Point2i m_position;
  Point2i m_offset;
};

} // namespace model


#endif // __OFFICE_H__