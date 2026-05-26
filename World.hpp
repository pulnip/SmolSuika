#pragma once

#include <tuple>
#include <vector>
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct Transform {
        Vec3 position;
    };
    struct SphereCollider {
        f32 radius;
    };
    struct Sprite {

    };
    struct Force {
        Vec3 value;
    };

    template<typename... Ts>
    class QueryView {
    private:
        std::tuple<std::vector<Ts>*...> storages;
        usize count;

    public:
        QueryView(std::vector<Ts>&... s) noexcept
            : storages(&s...)
            , count(std::get<0>(std::tuple{ s.size()... })) {}

        struct Iterator {
            std::tuple<std::vector<Ts>*...> storages;
            usize idx;

            std::tuple<Ts&...> operator*() noexcept {
                return std::apply(
                    [this](auto*... s) {
                        return std::tuple<Ts&...>((*s)[idx]...);
                    },
                    storages
                );
            }
            Iterator& operator++() noexcept {
                ++idx;
                return *this;
            }
            bool operator!=(const Iterator& other) const noexcept {
                return idx != other.idx;
            }
        };

        Iterator begin() noexcept {
            return { storages, 0 };
        }
        Iterator end() noexcept {
            return { storages, count };
        }
    };

    class World final {
    private:
        // ECS-style components
        std::vector<Transform> transforms;
        std::vector<SphereCollider> sphereColliders;
        std::vector<Sprite> sprites;
        std::vector<Force> forces;

    public:
        World() = default;
        ~World() = default;
        DECLARE_PINNED(World)

        template<typename... Ts>
        QueryView<Ts...> query() {
            return QueryView<Ts...>(getStorage<Ts>()...);
        }

    private:
        template<typename T>
        auto& getStorage() noexcept {
            if constexpr (std::is_same_v<T, Transform>)
                return transforms;
            else if constexpr (std::is_same_v<T, SphereCollider>)
                return sphereColliders;
            else if constexpr (std::is_same_v<T, Sprite>)
                return sprites;
            else if constexpr (std::is_same_v<T, Force>)
                return forces;
            else static_assert(sizeof(T) == 0, "Unknown component type");
        }
    };
}
