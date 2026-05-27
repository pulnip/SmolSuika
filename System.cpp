#include <cassert>
#include <cmath>
#include <stdexcept>
#include <wincodec.h>
#include <wrl/client.h>
#include "LinearAlgebra.hpp"
#include "OS.hpp"
#include "System.hpp"
#include "World.hpp"

namespace Smol
{
    void BoundarySystem::Update(World& world) {
        // Bounds
        constexpr u32 xMin = 0, yMin = 0;
        auto xMax = OS_->GetWidth(), yMax = OS_->GetHeight();

        constexpr f32 restitution = 0.2f, tangentialKeep = 0.7f;

        for (auto [transform, collider, velocity] : world.query<
            Transform, SphereCollider, Velocity
        >()) {
            auto pos = transform.position;
            auto vel = velocity.value;
            auto r = collider.radius;

            if (pos.x < xMin + r || xMax - r < pos.x) {
                pos.x = std::clamp<f32>(pos.x, xMin + r, xMax - r);

                vel.x = -vel.x * restitution;
                vel.y *= tangentialKeep;
            }
            if (pos.y < yMin + r || yMax - r < pos.y) {
                pos.y = std::clamp<f32>(pos.y, yMin + r, yMax - r);

                vel.y = -vel.y * restitution;
                vel.x *= tangentialKeep;
            }

            transform.position = pos;
            velocity.value = vel;
        }
    }

    void CollisionSystem::Update(World& world) {

    }

    void ForceSystem::Update(World& world) {
        
    }

    void IntegrateSystem::Update(World& world) {
        Vec2 gravity(0.0f, 980.0f); // 980 cm/s^2
        auto dt = world.GetDeltaTime();

        for (auto [transform, velocity] : world.query<
            Transform, Velocity
        >()) {
            auto pos = transform.position;
            auto vel = velocity.value;

            vel += gravity * dt;
            pos += vel * dt;

            transform.position = pos;
            velocity.value = vel;
        }
    }

    struct Bitmap {
        HBITMAP bitmap = nullptr;

        u32 width = 0, height = 0;
    };

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    Bitmap LoadImageFromFile(
        LPCWSTR filename,
        IWICImagingFactory& factory,
        IWICFormatConverter& converter
    ) {
        ComPtr<IWICBitmapDecoder> decoder;
        if (FAILED(factory.CreateDecoderFromFilename(
            filename,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        ))) {
            throw std::runtime_error("Failed to create bitmap decoder");
        }

        ComPtr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(
            0,
            frame.GetAddressOf()
        ))) {
            throw std::runtime_error("Failed to Get Frame");
        }

        if (FAILED(converter.Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeCustom
        ))) {
            throw std::runtime_error("Failed to initialize coverter");
        }

        UINT width = 0, height = 0;
        if (FAILED(frame->GetSize(
            &width,
            &height
        ))) {
            throw std::runtime_error("Failed to Get Frame Size");
        }

        BITMAPINFO bmi{
            .bmiHeader = {
                .biSize = sizeof(BITMAPINFOHEADER),
                .biWidth = static_cast<LONG>(width),
                .biHeight = -static_cast<LONG>(height),
                .biPlanes = 1,
                .biBitCount = 32,
                .biCompression = BI_RGB
            }
        };

        void* pvImageBits = nullptr;
        HDC hdc = GetDC(nullptr);
        HBITMAP bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
        ReleaseDC(nullptr, hdc);

        if (FAILED(converter.CopyPixels(
            nullptr,
            4 * width,
            4 * width * height,
            (BYTE*)pvImageBits
        ))) {
            throw std::runtime_error("Failed to copy pixels");
        }

        return Bitmap{
            .bitmap = bitmap,
            .width = width, .height = height
        };
    }

    class RenderSystem::Impl {
        int width = 0, height = 0;
        HDC hFrontDC = nullptr, hBackDC = nullptr;
        HBITMAP backBitmap = nullptr;
        HBITMAP defaultBitmap = nullptr;

        // No Resource Manager
        Bitmap suika;

    public:
        Impl();
        ~Impl();

        void Update(World&);
    };

    RenderSystem::RenderSystem()
        : impl(std::make_unique<Impl>()) {}

    RenderSystem::Impl::Impl() {
        auto hWnd = static_cast<HWND>(OS_->GetWindow());

        RECT rcClient = {};
        GetClientRect(hWnd, &rcClient);
        width = rcClient.right - rcClient.left;
        height = rcClient.bottom - rcClient.top;

        hFrontDC = GetDC(hWnd);
        hBackDC = CreateCompatibleDC(hFrontDC);
        backBitmap = CreateCompatibleBitmap(hFrontDC, width, height);

        defaultBitmap = static_cast<HBITMAP>(SelectObject(hBackDC, backBitmap));

        ComPtr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&factory)
        ))) {
            throw std::runtime_error("Failed to create WIC Imaging Factory");
        };

        ComPtr<IWICFormatConverter> converter;
        if (FAILED(factory->CreateFormatConverter(
            converter.GetAddressOf()
        ))) {
            throw std::runtime_error("Failed create format converter");
        }

        suika = LoadImageFromFile(
            L"suika0.png",
            *factory.Get(),
            *converter.Get()
        );
    }

    RenderSystem::~RenderSystem() = default;

    RenderSystem::Impl::~Impl() {
        SelectObject(hBackDC, defaultBitmap);

        DeleteObject(backBitmap);
        DeleteDC(hBackDC);

        auto hWnd = static_cast<HWND>(OS_->GetWindow());
        ReleaseDC(hWnd, hFrontDC);
    }

    void RenderSystem::Update(World& world) {
        impl->Update(world);
    }

    void RenderSystem::Impl::Update(World& world) {
        // Clear the back buffer
        ::PatBlt(hBackDC, 0, 0, width, height, WHITENESS);

        hBackDC;

        for (auto [transform, sprite] : world.query<Transform, Sprite>()) {
            auto hBitmapDC = CreateCompatibleDC(hBackDC);

            using enum Sprite::Type;
            Bitmap* selected = nullptr;
            switch (sprite.type) {
            case Watermelon:
                selected = &suika;
                break;
            default:
                std::unreachable();
            }

            assert(selected != nullptr);
            HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hBitmapDC, selected->bitmap));

            const auto x = static_cast<int>(transform.position.x - sprite.width / 2);
            const auto y = static_cast<int>(transform.position.y - sprite.height / 2);

            AlphaBlend(
                hBackDC,
                x, y, sprite.width, sprite.height,
                hBitmapDC,
                0, 0, selected->width, selected->height,
                BLENDFUNCTION{
                    .BlendOp = AC_SRC_OVER,
                    .BlendFlags = 0,
                    .SourceConstantAlpha = 0xFF,
                    .AlphaFormat = AC_SRC_ALPHA
                }
            );

            // Restore Bitmap handle
            SelectObject(hBitmapDC, hOldBitmap);
            DeleteDC(hBitmapDC);
        }

        // Copy MemoryDC(hBackDC) to Actual DC(hFrontDC)
        BitBlt(hFrontDC, 0, 0, width, height, hBackDC, 0, 0, SRCCOPY);
    }
}
