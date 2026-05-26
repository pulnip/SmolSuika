#include "System.hpp"
#include "World.hpp"

namespace Smol
{
    void BoundarySystem::Update(World& world) {
        for (auto [transform, collider] : world.query<Transform, SphereCollider>()) {

        }
    }

    void ForceSystem::Update(World& world) {
        for (auto [transform, force] : world.query<Transform, Force>()) {

        }
    }

    void IntegrateSystem::Update(World& world) {
        for (auto [transform, force] : world.query<Transform, Force>()) {

        }
    }

    void RenderSystem::Update(World& world) {
        for (auto [transform, sprite] : world.query<Transform, Sprite>()) {

        }
    }
}
