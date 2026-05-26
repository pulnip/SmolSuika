#pragma once

#include "Semantics.hpp"

namespace Smol
{
    class MainLoop {
    public:
        DECLARE_INTERFACE(MainLoop)

        virtual bool Initialize() { return true; }
        virtual bool Update() = 0;
        virtual bool Render() = 0;
        virtual void Finalize() {}
    };
}
