#pragma once

#include "System.hpp"

namespace Smol
{
    class CollisionSystem final : public System {
    public:
        void Update(World&) override;
    };
}
