#include "World.hpp"

namespace Smol
{
    World::EntityID World::CreateEntity(Vec2 position) {
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

        forces.push_back(Force{
            .value = Vec2(0.0f, 0.0f)
        });

        return transforms.size() - 1;
    }
}
