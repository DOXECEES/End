#pragma once

#include <type_traits>

template <typename E> class Flags
{
  public:
    using underlying_type = std::underlying_type_t<E>;

    constexpr Flags() noexcept : value_(0) {}

    constexpr Flags(E flag) noexcept
        : value_(static_cast<underlying_type>(flag))
    {
    }

    constexpr bool has(E flag) const noexcept
    {
        return (value_ & static_cast<underlying_type>(flag)) != 0;
    }

    constexpr explicit operator bool() const noexcept
    {
        return value_ != 0;
    }

    constexpr Flags operator|(Flags other) const noexcept
    {
        return Flags(value_ | other.value_);
    }

    constexpr Flags& operator|=(Flags other) noexcept
    {
        value_ |= other.value_;
        return *this;
    }

    constexpr Flags operator&(Flags other) const noexcept
    {
        return Flags(value_ & other.value_);
    }

    constexpr Flags& operator&=(Flags other) noexcept
    {
        value_ &= other.value_;
        return *this;
    }

  private:
    constexpr explicit Flags(underlying_type val) noexcept : value_(val) {}
    underlying_type value_;
};

#define DEFINE_ENUM_FLAG_OPERATORS(EnumName) \
    inline constexpr Flags<EnumName> operator|(EnumName lhs, EnumName rhs) noexcept { \
        return Flags<EnumName>(lhs) | Flags<EnumName>(rhs); \
    } \
    inline constexpr Flags<EnumName> operator&(EnumName lhs, EnumName rhs) noexcept { \
        return Flags<EnumName>(lhs) & Flags<EnumName>(rhs); \
    }
