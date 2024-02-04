#pragma once


namespace Engine
{

class OffscreenSurface final
{
public:
    OffscreenSurface();
    ~OffscreenSurface();
private:
    void* m_glRC;
};

}

#pragma once
