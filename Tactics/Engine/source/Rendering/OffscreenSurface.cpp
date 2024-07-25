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
    // for windows refactor, the hidden window would be the "Display"
    // THe chosen pixelformat descriptor the "Config" (need a DC for each surface?)
    //
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
    m_display = hWnd;
    m_surface = glRC;

    wglMakeCurrent(hDC, (HGLRC)m_surface);

    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);

    GLenum glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }
}


OffscreenSurface::~OffscreenSurface()
{
    wglMakeCurrent(NULL, NULL);
}


}
#else
#include <cassert>
#include <limits>
#include <EGL/egl.h>
#include <stdexcept>
#include <GL/glew.h>


namespace Engine
{

Display::Display() :
    m_display(nullptr)
{
    // TODO: separate open gl application/hidden window from surface for wgl and egl
    // Initialize EGL
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) {
        throw std::runtime_error("No default EGL display is found.");
    }
    EGLint major, minor;
    EGLBoolean result = eglInitialize(m_display, &major, &minor);
    if (!result)
    {
        m_display = EGL_NO_DISPLAY;
        throw std::runtime_error("Failed to initialize the default EGL display.");
    }
    if (!eglBindAPI(EGL_OPENGL_API))
    {
        throw std::runtime_error("Failed to initialize OpenGL for the display.");
    }
}

DisplayHandle Display::GetHandle() const
{
    return m_display;
}

PixelFormat Display::GetConfig(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t depth) const
{
    const  EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, red,
        EGL_GREEN_SIZE, green,
        EGL_BLUE_SIZE, blue,
        EGL_ALPHA_SIZE, alpha,
        EGL_DEPTH_SIZE, depth,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    // Choose the first EGL configuration that matches our requirements
    EGLint numConfigs = 0;
    EGLConfig eglCfg = nullptr;
    if (!eglChooseConfig(m_display, configAttribs, &eglCfg, 1, &numConfigs))
        throw std::runtime_error("Failed to configure the desired pixel format");
    if (numConfigs==0)
        throw std::runtime_error("The desired pixel format is not available");
    return eglCfg;
}

Display::~Display()
{
    if (m_display!= EGL_NO_DISPLAY)
    {
        eglTerminate(m_display);
    }
}

OffscreenSurface::OffscreenSurface(const Display& display, unsigned width, unsigned height, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t depth) :
    m_display(display)
{
    if ((width > std::numeric_limits<int>::max()) ||
        (height > std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("Offscreen surface extends to big");
    }
    auto config = m_display.GetConfig(red, green, blue, alpha, depth);
    // Set the desired pixel buffer configuration
    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, static_cast<int>(width),
        EGL_HEIGHT, static_cast<int>(height),
        EGL_NONE,
    };

    // Create a pixel buffer surface
    m_surface = eglCreatePbufferSurface(display.GetHandle(), config, pbufferAttribs);

    m_context = eglCreateContext(m_display.GetHandle(), config, EGL_NO_CONTEXT, nullptr);
    if (m_context == EGL_NO_CONTEXT)
    {
        Destroy();
        throw std::runtime_error("Failed to create an OpenGL Context for an offscreen surface");
    }

    if (!eglMakeCurrent(m_display.GetHandle(), m_surface, m_surface, m_context))
    {
        Destroy();
        throw std::runtime_error("Failed to create ");

    }

    // TODO: neither the linux nor the window implementation should bre responsible for glew, move to the app part
    GLenum glewError = glewInit();
    if ((GLEW_OK != glewError) && (GLEW_ERROR_NO_GLX_DISPLAY != glewError))
    {
        Destroy();
        throw std::runtime_error("Failed to initialize GLEW");
    }
}

OffscreenSurface::~OffscreenSurface()
{
    Destroy();
}

void OffscreenSurface::Destroy()
{
    if (m_context!=EGL_NO_CONTEXT)
    {
        eglDestroyContext(m_display.GetHandle(), m_context);
        m_context = EGL_NO_CONTEXT;
    }
    if (m_surface!=EGL_NO_SURFACE)
    {
        eglDestroySurface(m_display.GetHandle(), m_surface);
        m_surface = EGL_NO_SURFACE;
    }
}

}
#endif
