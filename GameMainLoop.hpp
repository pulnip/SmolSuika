#pragma once

#include "MainLoop.hpp"
#include "Timer.hpp"
#include "World.hpp"

namespace Smol
{
    class GameMainLoop final : public MainLoop {
    private:
        Timer timer;
        World world;

    public:
        GameMainLoop() = default;
        ~GameMainLoop() = default;

        bool Initialize();
        bool Update(const MouseState&);
        bool Render();
        void Finalize();
    };
}
