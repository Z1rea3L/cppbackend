#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

namespace gm {

template <typename T>
struct Point2 {
  Point2() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

  template <typename A, typename B>
  Point2(const A& a, const B& b) : x(static_cast<T>(a)), y(static_cast<T>(b)) {}

  template <typename U>
  Point2(const Point2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
  // field
  T x;
  T y;

  // operators overload
  Point2& operator+=(const Point2<T>& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  friend Point2 operator+(Point2<T> left, const Point2<T>& right) {
    left += right;
    return left;
  }

  Point2& operator+=(const T& scalar) {
    x += scalar;
    y += scalar;
    return *this;
  }

  friend Point2 operator+(Point2<T> left, const T& scalar) {
    left += scalar;
    return left;
  }

  friend Point2 operator+(const T& scalar, Point2<T> right) {
    right.x = scalar + right.x;
    right.y = scalar + right.y;
    return right;
  }

  Point2& operator-=(const T& scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
  }

  friend Point2 operator-(Point2<T> left, const T& scalar) {
    left -= scalar;
    return left;
  }

  friend Point2 operator-(const T& scalar, Point2<T> right) {
    right.x = scalar - right.x;
    right.y = scalar - right.y;
    return right;
  }

  Point2& operator*=(const T& scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  friend Point2 operator*(Point2<T> left, const T& scalar) {
    left *= scalar;
    return left;
  }

  friend Point2 operator*(const T& scalar, Point2<T> right) {
    right *= scalar;
    return right;
  }
};

using Point2i = Point2<int>;
using Point2d = Point2<double>;

template <typename T>
struct Rect {
  Rect() : left(static_cast<T>(0)), top(static_cast<T>(0)),  width(static_cast<T>(0)), height(static_cast<T>(0)) {} 
  Rect(Point2<T> pos, Point2<T> size) : left(pos.x), top(pos.y), width(size.x), height(size.y) {}
  Rect(T x, T y, T w, T h) : left(x), top(y), width(w), height(h) {}
  T left;
  T top;
  T width;
  T height;

  bool contains(const Point2<T>& point) const {
    // Not using 'std::min' and 'std::max' to avoid depending on '<algorithm>'
    const auto min = [](T a, T b) { return (a < b) ? a : b; };
    const auto max = [](T a, T b) { return (a < b) ? b : a; };

    // Rectangles with negative dimensions are allowed, so we must handle them correctly

    // Compute the real min and max of the rectangle on both axes
    const T minX = min(left, static_cast<T>(left + width));
    const T maxX = max(left, static_cast<T>(left + width));
    const T minY = min(top, static_cast<T>(top + height));
    const T maxY = max(top, static_cast<T>(top + height));

    return (point.x >= minX) && (point.x < maxX) && (point.y >= minY) && (point.y < maxY);
  }
};

}  // namespace gm

#endif  // __GEOMETRY_H__