#ifndef SIM_MATH_HPP
#define SIM_MATH_HPP

namespace math {

//! Vector of three components.
template<typename Type>
struct vec3
{
  Type _right;
  Type _up;
  Type _forward;

  //! Default constructor.
  vec3(Type right, Type up, Type forward)
      : _right(right), _up(up), _forward(forward)
  {}

  //! Scalar constructor.
  vec3(Type scalar)
      : vec3(scalar, scalar, scalar)
  {}

  //! Copy constructor.
  //! Copies all values from provided vector to this vector.
  vec3(const vec3 &rhs)
      : vec3(rhs._right, rhs._up, rhs._forward)
  {}

  bool positive() const
  {
    return _right >= 0 && _up >= 0 && _forward >= 0;
  }

  float magnitudeSquared() const
  {
    return std::pow(_right, 2) + std::pow(_up, 2) + std::pow(_forward, 2);
  }

  float magnitude() const
  {
    return std::sqrt(magnitudeSquared());
  }

  vec3<Type> absolute() const
  {
    return {std::abs(_right), std::abs(_up), std::abs(_forward)};
  }

  //! Evaluates multiply expression.
  vec3<Type> operator*(Type rhs) const {
    return vec3<Type>(
            _right * rhs,
            _up * rhs,
            _forward * rhs
    );
  }

  //! Evaluates multiply expression.
  vec3<Type> operator*(vec3<Type> rhs) const {
    return vec3<Type>(
            _right * rhs._right,
            _up * rhs._up,
            _forward * rhs._forward
    );
  }

  //! Evaluates divide expression.
  vec3<Type> operator/(Type rhs) const {
    return vec3<Type>(
            _right / rhs,
            _up / rhs,
            _forward / rhs
    );
  }

  vec3<Type> operator+(const vec3<Type>& rhs) const {
    return vec3<Type>(
            _right + rhs._right,
            _up + rhs._up,
            _forward + rhs._forward);
  }

  vec3<Type> operator-(const vec3<Type>& rhs) const {
    return vec3<Type>(
            _right - rhs._right,
            _up - rhs._up,
            _forward - rhs._forward);
  }

  vec3<Type>& operator+=(const vec3<Type>& rhs) {
    _right += rhs._right;
    _up += rhs._up;
    _forward += rhs._forward;
    return (*this);
  }

  vec3<Type>& operator-=(const vec3<Type>& rhs) {
    _right -= rhs._right;
    _up -= rhs._up;
    _forward -= rhs._forward;
    return (*this);
  }

  vec3<Type>& operator*=(const vec3<Type>& rhs) {
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
static const vec3d SidewaysVector{1,0,1};

} // namespace math

#endif