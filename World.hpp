#pragma once

#include <tuple>
#include <vector>
#include "CollisionSystem.hpp"
#include "Primitives.hpp"
#include "RenderSystem.hpp"
#include "Semantics.hpp"
#include "System.hpp"

namespace Smol
{
    using EntityID = usize;

    struct Rigidbody {
        f32 invMass = 1.0f;
        f32 restitution = 0.2f;
        f32 friction = 0.4f;
    };
    struct Sprite {
        enum class Type {
            Watermelon
        } type = Type::Watermelon;

        // Visual Size
        u32 width, height;
    };
    struct SphereCollider {
        f32 radius;
    };
    struct Transform {
        Vec2 position;
    };
    struct Velocity {
        Vec2 value = Vec2(0, 0);
    };

    template<bool IncludeID, bool IsConst, typename... Ts>
    class ViewImpl {
    private:
        template<typename T>
        using StoragePtr = std::conditional_t<
            IsConst,
            const std::vector<T>*,
            std::vector<T>*
        >;
        template<typename T>
        using ElemRef = std::conditional_t<
            IsConst,
            const T&,
            T&
        >;

        using ReturnType = std::conditional_t<
            IncludeID,
            std::tuple<EntityID, ElemRef<Ts>...>,
            std::tuple<ElemRef<Ts>...>
        >;

        using StorageTuple = std::tuple<StoragePtr<Ts>...>;

        StorageTuple storages;
        usize count;

    public:
        ViewImpl(
            std::conditional_t<IsConst, const std::vector<Ts>&, std::vector<Ts>&>... s
        ) noexcept
            : storages(&s...)
            , count(std::get<0>(std::tuple{ s.size()... })) {
        }

        ViewImpl& StartAt(usize start) {
            count = start;

            return *this;
        }

        struct Iterator {
            StorageTuple storages;
            usize idx;

            ReturnType operator*() noexcept {
                return std::apply(
                    [idx=idx](auto*... s) -> ReturnType {
                        if constexpr (IncludeID)
                            return ReturnType(idx, (*s)[idx]...);
                        else
                            return ReturnType((*s)[idx]...);
                    },
                    storages
                );
            }
            Iterator& operator++() noexcept { ++idx; return *this; }
            bool operator!=(const Iterator& other) const noexcept { return idx != other.idx; }
        };

        Iterator begin() noexcept { return { storages, 0 }; }
        Iterator end()   noexcept { return { storages, count }; }
    };

    template<typename... Ts>
    using QueryView = ViewImpl<false, false, Ts...>;
    template<typename... Ts>
    using ConstQueryView = ViewImpl<false, true, Ts...>;

    template<typename... Ts>
    using EnumerateView = ViewImpl<true, false, Ts...>;
    template<typename... Ts>
    using ConstEnumerateView = ViewImpl<true, true, Ts...>;

    class World final {
    private:
        // ECS-style components
        std::vector<Rigidbody> rigidbodies;
        std::vector<SphereCollider> sphereColliders;
        std::vector<Sprite> sprites;
        std::vector<Transform> transforms;
        std::vector<Velocity> velocities;

        // ECS-style systems
        BoundarySystem boundarySystem;
        CollisionSystem collisionSystem;
        ForceSystem forceSystem;
        IntegrateSystem integrateSystem;
        RenderSystem renderSystem;

        // Cache DeltaTime for ECS Systems
        f32 deltaTime = 0.0f;

    public:
        World() = default;
        ~World() = default;
        SMOL_DECLARE_PINNED(World)

        void Update(f32 deltaTime);

        EntityID CreateEntity(Vec2 position);

        template<typename... Ts>
        auto query() {
            return QueryView<Ts...>{ getStorage<Ts>()... };
        }
        template<typename... Ts>
        auto query() const {
            return ConstQueryView<Ts...>{ getStorage<Ts>()... };
        }
        template<typename... Ts>
        auto cquery() const {
            return ConstQueryView<Ts...>{ getStorage<Ts>()... };
        }

        template<typename... Ts>
        auto enumerate() {
            return EnumerateView<Ts...>{ getStorage<Ts>()... };
        }
        template<typename... Ts>
        auto enumerate() const {
            return ConstEnumerateView<Ts...>{ getStorage<Ts>()... };
        }
        template<typename... Ts>
        auto cenumerate() const {
            return ConstEnumerateView<Ts...>{ getStorage<Ts>()... };
        }

        f32 GetDeltaTime() const noexcept { return std::min(deltaTime, 0.1f); }

    private:
        template<typename T>
        auto& getStorage(this auto& self) noexcept {
            if constexpr (std::is_same_v<T, Rigidbody>)
                return self.rigidbodies;
            else if constexpr (std::is_same_v<T, SphereCollider>)
                return self.sphereColliders;
            else if constexpr (std::is_same_v<T, Sprite>)
                return self.sprites;
            else if constexpr (std::is_same_v<T, Transform>)
                return self.transforms;
            else if constexpr (std::is_same_v<T, Velocity>)
                return self.velocities;
            else static_assert(sizeof(T) == 0, "Unknown component type");
        }
    };
}
