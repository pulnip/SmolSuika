#include "GameMainLoop.hpp"
#include "Input.hpp"

namespace Smol
{
    bool GameMainLoop::Initialize() {
        return true;
    }

    bool GameMainLoop::Update(const MouseState& mouseState) {
        [[unlikely]] if (mouseState.leftPressed)
            world.CreateEntity(Vec2(mouseState.x, mouseState.y));

        // Manually sorted ECS System

        // [UpdateBefore(ForceSystem)]
        collisionSystem.Update(world);
        // [UpdateBefore(IntegrateSystem)]
        forceSystem.Update(world);

        integrateSystem.Update(world);
        // [UpdateAfter(IntegrateSystem)]
        boundarySystem.Update(world);

        return true;
    }

    bool GameMainLoop::Render() {
        renderSystem.Update(world);

        return true;
    }

    void GameMainLoop::Finalize() {

    }
}
