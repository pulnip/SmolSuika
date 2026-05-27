#include "World.hpp"

namespace Smol
{
    void World::Update(f32 deltaTime) {
        this->deltaTime = deltaTime;

        // Manually sorted ECS System

        // [UpdateBefore(ForceSystem)]
        collisionSystem.Update(*this);
        // [UpdateBefore(IntegrateSystem)]
        forceSystem.Update(*this);

        integrateSystem.Update(*this);
        // [UpdateAfter(IntegrateSystem)]
        boundarySystem.Update(*this);

        // Update After All System Done
        renderSystem.Update(*this);
    }

    EntityID World::CreateEntity(Vec2 position) {
        constexpr usize ENTITY_SIZE = 100;

        transforms.push_back(Transform{
            .position = position
        });
        sphereColliders.push_back(SphereCollider{
            .radius = ENTITY_SIZE / 2.0f
        });

        using enum Sprite::Type;

        sprites.push_back(Sprite{
            .type = Watermelon,
            .width = ENTITY_SIZE,
            .height = ENTITY_SIZE
        });

        velocities.push_back(Velocity{
            .value = Vec2(0.0f, 0.0f)
        });

        return transforms.size() - 1;
    }
}
