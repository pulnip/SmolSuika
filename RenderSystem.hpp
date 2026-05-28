#pragma once

#include "Primitives.hpp"
#include "System.hpp"

namespace Smol
{
    // No Renderer, so RenderSystem itself is a Renderer
    class RenderSystem final : public System {
    private:
        class Impl;
        RAII<Impl> impl;

    public:
        RenderSystem();
        ~RenderSystem();

        void Update(World&) override;
    };
}
