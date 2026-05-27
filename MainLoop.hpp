#pragma once

#include "Semantics.hpp"

namespace Smol
{
    struct MouseState;

    class MainLoop {
    public:
        SMOL_DECLARE_INTERFACE(MainLoop)

        virtual bool Initialize() { return true; }
        virtual bool Update(const MouseState&) = 0;
        virtual bool Render() = 0;
        virtual void Finalize() {}
    };
}
