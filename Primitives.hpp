#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace Smol
{
    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using usize = std::size_t;
    using isize = std::ptrdiff_t;

    using f32 = float;
    using f64 = double;

    using CStr = const char*;
    using CWStr = const wchar_t*;
    using Str = std::string;
    using WStr = std::wstring;
    using StrView = std::string_view;
    using WStrView = std::wstring_view;
    using Path = std::filesystem::path;

    template<typename T>
    using RAII = std::unique_ptr<T>;

    struct Vec2 {
        f32 x = 0.0f, y = 0.0f;

        constexpr auto& operator[](this auto& self, usize i) noexcept {
            switch (i) {
            case 0: return self.x;
            case 1: return self.y;
            default:
                std::unreachable();
            }
        }
    };


    struct Vec3 {
        f32 x = 0.0f, y = 0.0f, z = 0.0f;

        constexpr auto& operator[](this auto& self, usize i) noexcept {
            switch (i) {
            case 0: return self.x;
            case 1: return self.y;
            case 2: return self.z;
            default:
                std::unreachable();
            }
        }

        constexpr explicit operator Vec2() const noexcept {
            return { x, y };
        }
    };

    inline constexpr Vec3 toVec3(Vec2 v, f32 z = 0.0f) noexcept {
        return { v.x, v.y, z };
    }
}
