#pragma once

#include <cstdint>

#ifdef WIN32
    // TODO: The windows offscreen surface now also has to be refactored into a headless display
    #include <EGL/egl.h>
    using PixelFormat = PIXELFORMATDESCRIPTOR;
    using ContextHandle = HDC;
    using DisplayHandle = HWND;
    using SurfaceHandle = HGLRC;
#else
    #include <EGL/egl.h>
    using PixelFormat = EGLConfig;
    using ContextHandle = EGLContext;
    using DisplayHandle = EGLDisplay;
    using SurfaceHandle = EGLSurface;
#endif

namespace Engine
{

class Display
{
public:
    Display();
    ~Display();
    PixelFormat GetConfig(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0, uint8_t depth=16) const;
    DisplayHandle GetHandle() const;
private:
    DisplayHandle m_display = nullptr;
};

class OffscreenSurface final
{
public:
    OffscreenSurface(const Display& display, unsigned width=0, unsigned height=0, std::uint8_t red=8, std::uint8_t green=8, std::uint8_t blue=8, std::uint8_t alpha=0, std::uint8_t depth=16);
    ~OffscreenSurface();
private:
    void Destroy();
    const Display& m_display;
    ContextHandle m_context = nullptr;
    SurfaceHandle m_surface = nullptr;
};

}


