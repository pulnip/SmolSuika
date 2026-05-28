#pragma once

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
}
