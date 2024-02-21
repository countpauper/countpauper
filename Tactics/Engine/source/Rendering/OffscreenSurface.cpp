#include "Rendering/OffscreenSurface.h"
#ifdef WIN32
#include <windows.h>
#include <stdexcept>
#include <GL/glew.h>

namespace Engine
{

OffscreenSurface::OffscreenSurface() :
    m_glRC(0)
{
    TCHAR szWindowClass[] = "Offscreen";
    HINSTANCE moduleHandle = GetModuleHandle(NULL);
    WNDCLASSEX wcex;
    ATOM classRegistration = GetClassInfoEx(moduleHandle, szWindowClass, &wcex);
    if (!classRegistration)
    {
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC;
        wcex.lpfnWndProc = DefWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = moduleHandle;
        wcex.hIcon = NULL;
        wcex.hCursor = NULL;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = NULL;
        classRegistration = RegisterClassEx(&wcex);
        if (!classRegistration)
            throw std::runtime_error("Failed to register offscreen service class");
    }


    HWND hWnd = CreateWindow(szWindowClass, "Offscreen Surface", WS_DISABLED,
        0, 1, 0, 1, NULL, NULL, moduleHandle, NULL);
    if (!hWnd)
    {
        throw std::runtime_error("Failed to create offscreen window");
    }
    ShowWindow(hWnd, SW_HIDE);

    HDC hDC = GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cStencilBits = 8;
    pfd.cDepthBits = 24;


    int pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0)
    {
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);
        throw std::runtime_error("Failed to choose offscreen surface format");
    }

    if (SetPixelFormat(hDC, pf, &pfd) == FALSE)
    {
        throw std::runtime_error("Failed to set offscreen surface format");
    }

    // TODO move and delete on close
    HGLRC glRC = wglCreateContext(hDC);
    if (!glRC)
    {
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);
        throw std::runtime_error("Failed to create offscreen OpenGL context");
    }
    m_glRC = glRC;

    wglMakeCurrent(hDC, (HGLRC)m_glRC);
    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);

    GLenum glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        throw std::runtime_error("GLEW initialization error");
    }
}


OffscreenSurface::~OffscreenSurface()
{
    wglMakeCurrent(NULL, NULL);
}


}
#else
#include <cassert>

namespace Engine
{
    // TODO separate file
OffscreenSurface::OffscreenSurface()
{
}

OffscreenSurface::~OffscreenSurface()
{
}
}
#endif
