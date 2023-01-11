#ifndef TYPE_HPP
#define TYPE_HPP

namespace gfx {

template <typename T>
class BaseEnum {
public:
    T flags;
    BaseEnum() = default;
    BaseEnum(T flag) : flags(flag) {}
};

template <typename T>
class BaseFlag : public BaseEnum<T> {
public:
    T flags;
    BaseFlag() = default;
    BaseFlag(T flag) : flags(flag) {}
    constexpr bool operator==(T other) {
        return flags == other.flags;
    }
    constexpr bool operator!=(T other) {
        return flags != other.flags;
    }
    constexpr explicit operator bool() {
        return flags != 0;
    }
    constexpr explicit operator T() {
        return flags;
    }
    constexpr BaseFlag operator|(T other) {
        return {flags | other.flags};
    }
    constexpr BaseFlag operator&(T other) {
        return {flags & other.flags};
    }
    constexpr BaseFlag operator^(T other) {
        return {flags ^ other.flags};
    }
    constexpr BaseFlag operator~() {
        return {~flags};
    }
    constexpr BaseFlag& operator|=(T other) {
        flags |= other.flags;
        return *this;
    }
    constexpr BaseFlag& operator&=(T other) {
        flags &= other.flags;
        return *this;
    }
    constexpr BaseFlag& operator^=(T other) {
        flags ^= other.flags;
        return *this;
    }
};

} // namespace gfx

#endif