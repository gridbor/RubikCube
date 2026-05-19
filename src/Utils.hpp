#pragma once
#include <concepts>
#include <utility>
#include <cstdint>
#include <type_traits>


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

#define BITMASK_ENUM(EnumName) \
	template<> \
	struct is_bitmask_enum<EnumName> : std::true_type {};
