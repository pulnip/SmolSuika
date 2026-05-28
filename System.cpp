#include "LinearAlgebra.hpp"
#include "OS.hpp"
#include "System.hpp"
#include "World.hpp"

namespace Smol
{
    void BoundarySystem::Update(World& world) {
        // Bounds
        constexpr u32 xMin = 0, yMin = 0;
        auto xMax = OS_->GetWidth(), yMax = OS_->GetHeight();

        constexpr f32 restitution = 0.2f, tangentialKeep = 0.7f;

        for (auto [transform, collider, velocity] : world.query<
            Transform, SphereCollider, Velocity
        >()) {
            auto pos = transform.position;
            auto vel = velocity.value;
            auto r = collider.radius;

            if (pos.x < xMin + r || xMax - r < pos.x) {
                pos.x = std::clamp<f32>(pos.x, xMin + r, xMax - r);

                vel.x = -vel.x * restitution;
                vel.y *= tangentialKeep;
            }
            if (pos.y < yMin + r || yMax - r < pos.y) {
                pos.y = std::clamp<f32>(pos.y, yMin + r, yMax - r);

                vel.y = -vel.y * restitution;
                vel.x *= tangentialKeep;
            }

            transform.position = pos;
            velocity.value = vel;
        }
    }

    void ForceSystem::Update(World& world) {
        Vec2 gravity(0.0f, 980.0f); // 980 cm/s^2
        auto dt = world.GetDeltaTime();

        auto dvel = gravity * dt;

        for (auto [velocity] : world.query<Velocity>()) {
            velocity.value += dvel;
        }
    }

    void IntegrateSystem::Update(World& world) {
        auto dt = world.GetDeltaTime();

        for (auto [transform, velocity] : world.query<
            Transform, Velocity
        >()) {
            auto dpos = velocity.value * dt;
            transform.position += dpos;
        }
    }
}
