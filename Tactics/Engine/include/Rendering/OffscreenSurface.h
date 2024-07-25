#pragma once


namespace Engine
{

class OffscreenSurface final
{
public:
    OffscreenSurface();
    ~OffscreenSurface();
private:
    void* m_display; // Render context for windows, display for egl
    void *m_surface;
};

}

#pragma once
