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
    m_display = nullptr; // TODO, remember window or DC if they are not destroyed?
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

#include <EGL/egl.h>
#include <stdexcept>
#include <GL/glew.h>


namespace Engine
{
// TODO separate file
OffscreenSurface::OffscreenSurface()
{
static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    // Set the desired pixel buffer configuration
    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, static_cast<int>(100),   // TODO: configurable?
        EGL_HEIGHT, static_cast<int>(100),
        EGL_NONE,
    };

    // TODO: separate open gl application/hidden window from surface for wgl and egl
    // Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    EGLBoolean result = eglInitialize(eglDpy, &major, &minor);
    EGLint err = eglGetError();
    m_display = eglDpy;

    // Choose the first EGL configuration that matches our requirements
    EGLint numConfigs;
    static EGLConfig eglCfg;
    result = eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    err = eglGetError();

    result = eglBindAPI(EGL_OPENGL_API);
    err = eglGetError();
    // Create a pixel buffer surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);
    m_surface = eglSurf;

    EGLContext context = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, nullptr);
    result = eglMakeCurrent(eglDpy, eglSurf, eglSurf, context);
    err = eglGetError();

    // TODO: neither the linux nor the window implementation should bre responsible for glew, move to the app part
    GLenum glewError = glewInit();
    if ((GLEW_OK != glewError) && (GLEW_ERROR_NO_GLX_DISPLAY != glewError))
    {
        eglDestroySurface((EGLDisplay)m_display, (EGLSurface)m_surface);
        m_surface = nullptr;
        throw std::runtime_error("Failed to initialize GLEW");
    }
}

OffscreenSurface::~OffscreenSurface()
{
    // TODO eglDestroyContext(m_context);
    eglDestroySurface((EGLDisplay)m_display, (EGLSurface)m_surface);
}
}
#endif
