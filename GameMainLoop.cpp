#include "GameMainLoop.hpp"

namespace Smol
{
    bool GameMainLoop::Initialize() {
        world.CreateEntity(Vec2(100, 100));

        return true;
    }

    bool GameMainLoop::Update() {
        // Manually sorted ECS System
        boundarySystem.Update(world);
        forceSystem.Update(world);

        integrateSystem.Update(world);

        return true;
    }

    bool GameMainLoop::Render() {
        renderSystem.Update(world);

        return true;
    }

    void GameMainLoop::Finalize() {

    }
}
