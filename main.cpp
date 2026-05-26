#include "GameMainLoop.hpp"
#include "OS.hpp"

int main(int, char* []) {
    using namespace Smol;

    WindowConfig windowConfig{
        .title = L"SmolSuika",
        .width = 800,
        .height = 600
    };
    OS os(windowConfig);
    GameMainLoop mainLoop;

    os.Run(mainLoop);

    return EXIT_SUCCESS;
}
