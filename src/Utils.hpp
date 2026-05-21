#pragma once
#include <concepts>
#include <utility>
#include <cstdint>
#include <type_traits>
#include <string>


template<typename T>
struct is_bitmask_enum : std::false_type {};

template<typename T>
inline constexpr bool is_bitmask_enum_v = is_bitmask_enum<T>::value;

template<typename T>
concept BitmaskEnum = std::is_enum_v<T> && is_bitmask_enum_v<T>;

namespace {
	template<typename T>
	constexpr std::underlying_type_t<T> to_underlying(T value) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}
}


template<BitmaskEnum T>
constexpr T operator|(T lhs, T rhs) noexcept
{
	return static_cast<T>(::to_underlying(lhs) | ::to_underlying(rhs));
}

template<BitmaskEnum T>
constexpr T operator&(T lhs, T rhs) noexcept
{
	return static_cast<T>(::to_underlying(lhs) & ::to_underlying(rhs));
}

template<BitmaskEnum T>
constexpr T operator^(T lhs, T rhs) noexcept
{
	return static_cast<T>(::to_underlying(lhs) ^ ::to_underlying(rhs));
}

template<BitmaskEnum T>
constexpr T operator~(T value) noexcept
{
	return static_cast<T>(~::to_underlying(value));
}


template<BitmaskEnum T>
constexpr T& operator|=(T& lhs, T rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

template<BitmaskEnum T>
constexpr T& operator&=(T& lhs, T rhs) noexcept
{
	lhs = lhs & rhs;
	return lhs;
}

template<BitmaskEnum T>
constexpr T& operator^=(T& lhs, T rhs) noexcept
{
	lhs = lhs ^ rhs;
	return lhs;
}

template<BitmaskEnum T>
constexpr bool BitmaskHasFlag(T lhs, T rhs) noexcept
{
	return (lhs & rhs) == rhs;
}
template<BitmaskEnum T>
constexpr bool BitmaskOnlyFlag(T lhs, T rhs) noexcept
{
	return (lhs & ~rhs) == static_cast<T>(0);
}

#define BITMASK_ENUM(EnumName) \
	template<> \
	struct is_bitmask_enum<EnumName> : std::true_type {};


#include <glm/glm.hpp>


inline const float HS = 0.5f;
inline const float MUL = 2.0f;
inline const glm::vec3 FRONT_COLOR{ 0.f, 0.f, 0.66f };
inline const glm::vec3 BACK_COLOR{ 0.f, 0.66f, 0.f };
inline const glm::vec3 LEFT_COLOR{ 0.66f, 0.f, 0.f };
inline const glm::vec3 RIGHT_COLOR{ 0.66f, 0.33f, 0.f };
inline const glm::vec3 TOP_COLOR{ 0.66f, 0.66f, 0.66f };
inline const glm::vec3 BOTTOM_COLOR{ 0.66f, 0.66f, 0.f };

inline const float ROTATE_TIME = 0.33f;
static inline float g_rotateDeg = 90.f;
inline const float PITCH_LIMIT = glm::radians(89.f);

template<typename T>
static inline std::string VecToString(const T& vec)
{
	std::string s = "( ";
	for (int i = 0; i < vec.length(); i++) {
		s += (i == 0 ? "" : ", ") + std::to_string(vec[i]);
	}
	s += " )";
	return s.c_str();
}
