#pragma once

#include "MainLoop.hpp"
#include "System.hpp"
#include "World.hpp"

namespace Smol
{
    class GameMainLoop final : public MainLoop {
    private:
        World world;

        BoundarySystem boundarySystem;
        CollisionSystem collisionSystem;
        ForceSystem forceSystem;
        IntegrateSystem integrateSystem;
        RenderSystem renderSystem;

    public:
        GameMainLoop() = default;
        ~GameMainLoop() = default;

        bool Initialize();
        bool Update(const MouseState&);
        bool Render();
        void Finalize();
    };
}
