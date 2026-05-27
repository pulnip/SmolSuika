#pragma once

#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class World;

    // ECS System Interface
    class System {
    public:
        SMOL_DECLARE_INTERFACE(System)

        virtual void Update(World&) = 0;
    };

    class BoundarySystem final : public System {
    public:
        void Update(World&) override;
    };

    class CollisionSystem final : public System {
    public:
        void Update(World&) override;
    };

    class ForceSystem final : public System {
    public:
        void Update(World&) override;
    };

    class IntegrateSystem final : public System {
    public:
        void Update(World&) override;
    };

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
