#pragma once

#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
	struct WindowConfig {
		const wchar_t* title;
		u32 width = 800, height = 600;
	};

    class MainLoop;

	class OS final {
	private:
        class Impl;
        RAII<Impl> impl;

		static OS* singleton;

	public:
		OS(const WindowConfig&);
		~OS();
        SMOL_DECLARE_PINNED(OS)

		void Run(MainLoop& mainLoop);

        inline static OS* Get() noexcept { return singleton; }

        u32 GetWidth() const noexcept;
        u32 GetHeight() const noexcept;

        void* GetWindow() const noexcept;
	};
}

#define OS_ Smol::OS::Get()
