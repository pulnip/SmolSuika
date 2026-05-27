#include <chrono>
#include "INC_Windows.h"
#include <windowsx.h>
#include "Input.hpp"
#include "MainLoop.hpp"
#include "OS.hpp"
#include "Timer.hpp"

namespace Smol
{
    OS* OS::singleton = nullptr;

    class OS::Impl final {
    private:
        HWND hWnd = nullptr;
        u32 width = 0, height = 0;

        // Non-stop timer
        Timer sysTimer;

        MouseState mouse;

    public:
        Impl(const WindowConfig&);
        ~Impl();

        void Run(MainLoop&);

        u32 GetWidth() const noexcept { return width; }
        u32 GetHeight() const noexcept { return height; }

        HWND GetWindow() const noexcept { return hWnd; }

    private:
        static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
        LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

        bool processEvents();

        void onMouseMove(i32 x, i32 y) {
            mouse.dx += x - mouse.x;
            mouse.dy += y - mouse.y;
            mouse.x = x;
            mouse.y = y;
        }
        void onMouseLeftDown(int x, int y) {
            mouse.leftDown = true;
            mouse.leftPressed = true;
            mouse.x = x;
            mouse.y = y;
        }
        void onMouseLeftUp(int x, int y) {
            mouse.leftDown = false;
            mouse.leftReleased = true;
        }
        void consumeFrameInput() {
            mouse.dx = mouse.dy = 0;
            mouse.leftPressed = mouse.leftReleased = false;
        }
    };

    OS::OS(const WindowConfig& cfg)
        : impl(std::make_unique<Impl>(cfg))
    {
        singleton = this;
    }

    OS::Impl::Impl(const WindowConfig& windowConfig) {
        WNDCLASSEX wc{
            sizeof(WNDCLASSEX),
            CS_CLASSDC,
            WndProc,
            0L, 0L,
            GetModuleHandle(NULL),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            L"SmolWindowClass", // lpszClassName
            nullptr
        };

        RegisterClassEx(&wc);

        RECT rect{
            0,
            0,
            static_cast<LONG>(windowConfig.width),
            static_cast<LONG>(windowConfig.height)
        };
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        hWnd = CreateWindow(
            wc.lpszClassName,
            windowConfig.title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left, rect.bottom - rect.top,
            nullptr,
            nullptr,
            wc.hInstance,
            this
        );
        if (hWnd == nullptr) {
            throw std::runtime_error("Failed to create window");
        }

        constexpr bool immediateShow = true;
        if (immediateShow) {
            ShowWindow(hWnd, SW_SHOW);
            UpdateWindow(hWnd);
        }
    }

    OS::~OS() {
        singleton = nullptr;
    }

    OS::Impl::~Impl() {
        if (hWnd != nullptr) {
            DestroyWindow(hWnd);
            hWnd = nullptr;
        }
    }

    void OS::Run(MainLoop& mainLoop) {
        impl->Run(mainLoop);
    }

    void OS::Impl::Run(MainLoop& mainLoop) {
        if (!mainLoop.Initialize())
            return;

        sysTimer.Reset();

        while (true) {
            sysTimer.NewFrame();

            const auto inputSnapshot = mouse;
            consumeFrameInput();

            [[unlikely]] if (!processEvents())
                break;

            [[unlikely]] if (!mainLoop.Update(inputSnapshot))
                break;

            [[unlikely]] if (!mainLoop.Render())
                break;
        }

        mainLoop.Finalize();
    }

    bool OS::Impl::processEvents() {
        bool keepRunning = true;
        MSG msg{};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            switch (msg.message) {
            case WM_QUIT:
                keepRunning = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return keepRunning;
    }

    LRESULT CALLBACK OS::Impl::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(
                hWnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(cs->lpCreateParams)
            );
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        auto self = reinterpret_cast<Impl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        // before WM_NCCREATE
        if (self == nullptr)
            return DefWindowProc(hWnd, msg, wParam, lParam);

        return self->HandleMessage(hWnd, msg, wParam, lParam);
    }

    LRESULT OS::Impl::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_MOUSEMOVE:
            onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_LBUTTONDOWN:
            SetCapture(hWnd);
            onMouseLeftDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_LBUTTONUP:
            ReleaseCapture();
            onMouseLeftUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        return NULL;
    }

    u32 OS::GetWidth() const noexcept { return impl->GetWidth(); }
    u32 OS::GetHeight() const noexcept { return impl->GetHeight(); }

    void* OS::GetWindow() const noexcept { return impl->GetWindow(); }
}
