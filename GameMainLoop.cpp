#include "GameMainLoop.hpp"
#include "Input.hpp"

namespace Smol
{
    bool GameMainLoop::Initialize() {
        timer.Reset();

        return true;
    }

    bool GameMainLoop::Update(const MouseState& mouseState) {
        timer.NewFrame();

        [[unlikely]] if (mouseState.leftPressed) {
            Vec2 screenPos(f32(mouseState.x), f32(mouseState.y));
            world.CreateEntity(screenPos);
        }

        auto deltaTime = static_cast<f32>(timer.GetDeltaTime());
        world.Update(deltaTime);

        return true;
    }

    bool GameMainLoop::Render() {
        // Delegate Rendering Task to RenderSystem
        return true;
    }

    void GameMainLoop::Finalize() {

    }
}
