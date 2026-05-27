#pragma once

#include <cassert>
#include <numbers>
#include <type_traits>
#include "Primitives.hpp"

namespace Smol
{
    inline constexpr Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }
    inline constexpr Vec2& operator+=(Vec2& lhs, Vec2 rhs) noexcept {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator-(Vec2 v) noexcept {
        return { -v.x, -v.y };
    }
    inline constexpr Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept {
        return { lhs.x - rhs.x, lhs.y - rhs.y };
    }
    inline constexpr Vec2& operator-=(Vec2& lhs, Vec2 rhs) noexcept {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(Vec2 lhs, Vec2 rhs) noexcept {
        return { lhs.x * rhs.x, lhs.y * rhs.y };
    }
    inline constexpr Vec2& operator*=(Vec2& lhs, Vec2 rhs) noexcept {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(f32 f, Vec2 v) noexcept {
        return { f * v.x, f * v.y };
    }
    inline constexpr Vec2 operator*(Vec2 v, f32 f) noexcept {
        return f * v;
    }
    inline constexpr Vec2& operator*=(Vec2& v, f32 f) noexcept {
        v.x *= f;
        v.y *= f;
        return v;
    }
    inline constexpr Vec2 operator/(Vec2 v, f32 f) noexcept {
        return { v.x / f, v.y / f };
    }
    inline constexpr Vec2& operator/=(Vec2& v, f32 f) noexcept {
        v.x /= f;
        v.y /= f;
        return v;
    }

    inline constexpr bool operator==(Vec2 lhs, Vec2 rhs) noexcept {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline constexpr f32 dot(Vec2 lhs, Vec2 rhs) noexcept {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }
    inline constexpr f32 normSquared(Vec2 v) noexcept {
        return dot(v, v);
    }
    inline f32 norm(Vec2 v) noexcept {
        return std::sqrt(normSquared(v));
    }
    inline Vec2 normalize(Vec2 v) noexcept {
        return v / norm(v);
    }
    inline constexpr f32 cross(Vec2 lhs, Vec2 rhs) noexcept {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

    inline constexpr Vec3 zeros() noexcept {
        return { 0.0f, 0.0f, 0.0f };
    }
    inline constexpr Vec3 ones() noexcept {
        return { 1.0f, 1.0f, 1.0f };
    }
    inline constexpr Vec3 unitX() noexcept {
        return { 1.0f, 0.0f, 0.0f };
    }
    inline constexpr Vec3 unitY() noexcept {
        return { 0.0f, 1.0f, 0.0f };
    }
    inline constexpr Vec3 unitZ() noexcept {
        return { 0.0f, 0.0f, 1.0f };
    }

    inline constexpr Vec3 operator+(Vec3 lhs, Vec3 rhs) noexcept {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }
    inline constexpr Vec3& operator+=(Vec3& lhs, Vec3 rhs) noexcept {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator-(Vec3 v) noexcept {
        return { -v.x, -v.y, -v.z };
    }
    inline constexpr Vec3 operator-(Vec3 lhs, Vec3 rhs) noexcept {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }
    inline constexpr Vec3& operator-=(Vec3& lhs, Vec3 rhs) noexcept {
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(Vec3 lhs, Vec3 rhs) noexcept {
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }
    inline constexpr Vec3& operator*=(Vec3& lhs, Vec3 rhs) noexcept {
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(f32 f, Vec3 v) noexcept {
        return {
            f * v.x,
            f * v.y,
            f * v.z
        };
    }
    inline constexpr Vec3 operator*(Vec3 v, f32 f) noexcept {
        return f * v;
    }
    inline constexpr Vec3& operator*=(Vec3& v, f32 f) noexcept {
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    inline constexpr Vec3 operator/(Vec3 v, f32 f) noexcept {
        return {
            v.x / f,
            v.y / f,
            v.z / f
        };
    }
    inline constexpr Vec3& operator/=(Vec3& v, f32 f) noexcept {
        v.x /= f;
        v.y /= f;
        v.z /= f;
        return v;
    }

    inline constexpr bool operator==(Vec3 lhs, Vec3 rhs) noexcept {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    inline constexpr f32 dot(Vec3 lhs, Vec3 rhs) noexcept {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }
    inline constexpr f32 normSquared(Vec3 v) noexcept {
        return dot(v, v);
    }
    inline f32 norm(Vec3 v) noexcept {
        return std::sqrt(normSquared(v));
    }
    inline Vec3 normalize(Vec3 v) noexcept {
        return v / norm(v);
    }
    inline constexpr Vec3 cross(Vec3 lhs, Vec3 rhs) noexcept {
        return {
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x
        };
    }

    inline constexpr bool overlap(
        f32 m1, f32 M1,
        f32 m2, f32 M2,
        f32 epsilon = 0
    ) noexcept {
        assert(epsilon >= 0);
        if (m1 > m2) {
            std::swap(m1, m2);
            std::swap(M1, M2);
        }

        return M1 >= m2 - epsilon;
    }

    inline constexpr f64 toRadian(f64 degree) noexcept {
        return degree * (std::numbers::pi / 180.0);
    }
}
