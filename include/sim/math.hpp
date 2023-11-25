#ifndef SIM_MATH_HPP
#define SIM_MATH_HPP

#include <cmath>

namespace math
{

//! Vector of three components.
template<typename Type>
struct vec3
{
  Type _right;
  Type _up;
  Type _forward;

  //! Default constructor.
  //! @param right Scalar value for right axis.
  //! @param up Scalar value for up axis.
  //! @param forward Scalar value for forward axis.
  vec3(Type right, Type up, Type forward)
      : _right(right), _up(up), _forward(forward)
  {
  }

  //! Scalar constructor. Initializes all axes to scalar value provided.
  //! @param scalar Scalar value.
  explicit vec3(Type scalar)
      : vec3(scalar, scalar, scalar)
  {
  }

  //! Copy constructor.
  //! Copies all values from provided vector to this vector.
  vec3(const vec3& rhs)
      : vec3(rhs._right, rhs._up, rhs._forward)
  {
  }

  //! Optimisation in cases where magnitude doesn't have to be precise, such as vector comparison.
  //! @returns Vector magnitude.
  [[nodiscard]] float magnitudeSquared() const
  {
    return std::pow(_right, 2) + std::pow(_up, 2) + std::pow(_forward, 2);
  }

  //! @returns Vector magnitude.
  [[nodiscard]] float magnitude() const
  {
    return std::sqrt(magnitudeSquared());
  }

  //! @returns Absolute vector.
  [[nodiscard]] vec3<Type> absolute() const
  {
    return {std::abs(_right), std::abs(_up), std::abs(_forward)};
  }

  //! Vector-scalar multiplication.
  //! @param rhs Scalar.
  [[nodiscard]] vec3<Type> operator*(Type rhs) const
  {
    return vec3<Type>(
      _right * rhs,
      _up * rhs,
      _forward * rhs);
  }

  //! Vector-scalar division.
  //! @param rhs Scalar.
  [[nodiscard]] vec3<Type> operator/(Type rhs) const
  {
    return vec3<Type>(
      _right / rhs,
      _up / rhs,
      _forward / rhs);
  }

  //! Vector-vector multiplication.
  //! @param rhs Vector.
  [[nodiscard]] vec3<Type> operator*(vec3<Type> rhs) const
  {
    return vec3<Type>(
      _right * rhs._right,
      _up * rhs._up,
      _forward * rhs._forward);
  }

  //! Vector-vector addition.
  //! @param rhs Vector.
  [[nodiscard]] vec3<Type> operator+(const vec3<Type>& rhs) const
  {
    return vec3<Type>(
      _right + rhs._right,
      _up + rhs._up,
      _forward + rhs._forward);
  }

  //! Vector-vector subtraction.
  //! @param rhs Vector.
  [[nodiscard]] vec3<Type> operator-(const vec3<Type>& rhs) const
  {
    return vec3<Type>(
      _right - rhs._right,
      _up - rhs._up,
      _forward - rhs._forward);
  }

  //! Vector-vector addition and assignment.
  //! @param rhs Vector.
  vec3<Type>& operator+=(const vec3<Type>& rhs)
  {
    _right += rhs._right;
    _up += rhs._up;
    _forward += rhs._forward;
    return (*this);
  }

  //! Vector-vector subtraction and assignment.
  //! @param rhs Vector.
  vec3<Type>& operator-=(const vec3<Type>& rhs)
  {
    _right -= rhs._right;
    _up -= rhs._up;
    _forward -= rhs._forward;
    return (*this);
  }

  //! Vector-vector multiplication and assignment.
  //! @param rhs Vector.
  vec3<Type>& operator*=(const vec3<Type>& rhs)
  {
    _right *= rhs._right;
    _up *= rhs._up;
    _forward *= rhs._forward;
    return (*this);
  }
};

//! Vector of three integer components.
using vec3i = vec3<int>;
//! Vector of three float components.
using vec3f = vec3<float>;
//! Vector of three double components.
using vec3d = vec3<double>;

//! Zero vector.
static const vec3d ZeroVector(0.0);

//! Right vector.
static const vec3d RightVector{1, 0, 0};
//! Upward vector.
static const vec3d UpwardVector{0, 1, 0};
// Forward vector.
static const vec3d ForwardVector{0, 0, 1};
// Sideways vector.
static const vec3d SidewaysVector{1, 0, 1};

}// namespace math

#endif
