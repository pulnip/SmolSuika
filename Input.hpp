#pragma once

#include "Primitives.hpp"

namespace Smol
{
    struct MouseState {
        i32 x = 0, y = 0;
        i32 dx = 0, dy = 0;
        bool leftDown = false;
        // Rising / Falling Edge
        bool leftPressed = false, leftReleased = false;
    };
}
