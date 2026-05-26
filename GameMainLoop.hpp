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
        ForceSystem forceSystem;
        IntegrateSystem integrateSystem;
        RenderSystem renderSystem;

    public:
        GameMainLoop() = default;
        ~GameMainLoop() = default;

        bool Initialize();
        bool Update();
        bool Render();
        void Finalize();
    };
}
