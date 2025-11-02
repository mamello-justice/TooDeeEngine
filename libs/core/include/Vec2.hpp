#pragma once

#include <math.h>
#include <SFML/Graphics.hpp>

template <typename T>
class Vec2
{
public:
	T x = 0;
	T y = 0;

	/**
	 * @brief Default constructor initializing vector to (0,0)
	 */
	Vec2() = default;

	/**
	 * @brief Constructor initializing vector to (xIn, yIn)
	 *
	 * @param xIn X coordinate
	 * @param yIn Y coordinate
	 */
	Vec2(T xIn, T yIn) : x(xIn), y(yIn) {}

	/**
	 * @brief Constructor to convert from sf::Vector2
	 *
	 * @param v SFML vector to convert from
	 */
	Vec2(const sf::Vector2<T> &v) : x(v.x), y(v.y) {}

	/**
	 * @brief Constructor to convert from sf::Vector2i
	 *
	 * @param v SFML integer vector to convert from
	 */
	Vec2(const sf::Vector2i &v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

	/**
	 * @brief Conversion operator to sf::Vector2
	 *
	 * @return sf::Vector2<T> Converted SFML vector
	 */
	operator sf::Vector2<T>() { return sf::Vector2<T>(x, y); }

	/**
	 * @brief Conversion operator to sf::Vector2
	 *
	 * @return sf::Vector2<T> Converted SFML vector
	 */
	operator sf::Vector2<T>() const { return sf::Vector2<T>(x, y); }

	/**
	 * @brief Addition operator
	 *
	 * @param rhs Right-hand side vector
	 * @return Vec2 Resulting vector after addition
	 */
	inline Vec2 operator+(const Vec2 &rhs) const
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	/**
	 * @brief Subtraction operator
	 *
	 * @param rhs Right-hand side vector
	 * @return Vec2 Resulting vector after subtraction
	 */
	inline Vec2 operator-(const Vec2 &rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	/**
	 * @brief Division by scalar operator
	 *
	 * @param val Scalar value to divide by
	 * @return Vec2 Resulting vector after division
	 */
	inline Vec2 operator/(T val) const
	{
		return Vec2(x / val, y / val);
	}

	/**
	 * @brief Component-wise division operator
	 *
	 * @param rhs Right-hand side vector
	 * @return Vec2 Resulting vector after component-wise division
	 */
	inline Vec2 operator/(const Vec2 &rhs) const
	{
		return Vec2(x / rhs.x, y / rhs.y);
	}

	/**
	 * @brief Multiplication by scalar operator
	 *
	 * @param val Scalar value to multiply by
	 * @return Vec2 Resulting vector after multiplication
	 */
	inline Vec2 operator*(T val) const
	{
		return Vec2(x * val, y * val);
	}

	/**
	 * @brief Component-wise multiplication operator
	 *
	 * @param rhs Right-hand side vector
	 * @return Vec2 Resulting vector after component-wise multiplication
	 */
	inline Vec2 operator*(const Vec2 &rhs)
	{
		return Vec2(x * rhs.x, y * rhs.y);
	}

	/**
	 * @brief Equality operator
	 *
	 * @param rhs Right-hand side vector
	 * @return true if vectors are equal
	 */
	inline bool operator==(const Vec2 &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	/**
	 * @brief Inequality operator
	 *
	 * @param rhs Right-hand side vector
	 * @return true if vectors are not equal
	 */
	inline bool operator!=(const Vec2 &rhs) const
	{
		return !(*this == rhs);
	}

	/**
	 * @brief Addition assignment operator
	 *
	 * @param rhs Right-hand side vector
	 */
	inline void operator+=(const Vec2 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	/**
	 * @brief Subtraction assignment operator
	 *
	 * @param rhs Right-hand side vector
	 */
	inline void operator-=(const Vec2 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	/**
	 * @brief Multiplication assignment operator
	 *
	 * @param val Scalar value to multiply by
	 */
	inline void operator*=(T val)
	{
		x *= val;
		y *= val;
	}

	/**
	 * @brief Division assignment operator
	 *
	 * @param val Scalar value to divide by
	 */
	inline void operator/=(T val)
	{
		x /= val;
		y /= val;
	}

	/**
	 * @brief Boolean conversion operator
	 *
	 * @return true if either x or y is non-zero
	 */
	inline operator bool() const
	{
		return x or y;
	}

	/**
	 * @brief Calculate distance to another vector
	 *
	 * @param rhs Right-hand side vector
	 * @return Distance to the other vector
	 */
	inline T dist(const Vec2 &rhs) const
	{
		return sqrt(distSq(rhs));
	}

	/**
	 * @brief Calculate squared distance to another vector
	 *
	 * @param rhs Right-hand side vector
	 * @return Squared distance to the other vector
	 */
	inline T distSq(const Vec2 &rhs) const
	{
		return (x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y);
	}

	/**
	 * @brief Calculate length (magnitude) of the vector
	 *
	 * @return Length of the vector
	 */
	inline T length() const
	{
		return sqrt(x * x + y * y);
	}

	/**
	 * @brief Normalize the vector (make it unit length)
	 *
	 * @return Normalized vector
	 */
	inline Vec2 normalize() const
	{
		T l = length();
		return Vec2(x / l, y / l);
	}

	/**
	 * @brief Get the absolute value of the vector components
	 *
	 * @return Vector with absolute component values
	 */
	inline Vec2 abs() const
	{
		return Vec2(x < 0 ? -x : x, y < 0 ? -y : y);
	}
};

using Vec2f = Vec2<float>;
using Vec2u = Vec2<unsigned int>;

static_assert(std::is_default_constructible_v<Vec2f>);
