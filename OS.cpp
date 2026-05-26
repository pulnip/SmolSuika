#include <chrono>
#include "INC_Windows.h"
#include "MainLoop.hpp"
#include "OS.hpp"
#include "Timer.hpp"

LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Smol
{
    OS* OS::singleton = nullptr;

    class OS::Impl final {
    private:
        HWND hWnd = nullptr;
        u32 width = 0, height = 0;

        // Non-stop timer
        Timer sysTimer;

    public:
        Impl(const WindowConfig&);
        ~Impl();

        void Run(MainLoop&);

        u32 GetWidth() const noexcept { return width; }
        u32 GetHeight() const noexcept { return height; }

    private:
        friend LRESULT CALLBACK::MyWndProc(HWND, UINT, WPARAM, LPARAM);

        bool processEvents();
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
            MyWndProc,
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
            nullptr
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

            [[unlikely]] if (!processEvents())
                break;

            [[unlikely]] if (!mainLoop.Update())
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

    u32 OS::GetWidth() const noexcept { return impl->GetWidth(); }
    u32 OS::GetHeight() const noexcept { return impl->GetHeight(); }
}

LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE: {
        PostQuitMessage(0);
    } break;
    case WM_LBUTTONDOWN: {

    } break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return NULL;
}
