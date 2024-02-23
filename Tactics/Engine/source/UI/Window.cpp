#include "UI/Window.h"
#include "UI/Debug.h"
#include "Rendering/Color.h"
#include "UI/WindowMessages.h"
#include "Utility/Singleton.h"
#include "UI/Application.h"
#include <GL/glew.h>
#include <GL/glut.h>
#undef GetObject
#include <assert.h>

namespace Engine
{

Singleton<Application> app;

void Window::Init(void)
{
    GLenum err =glewInit();
    if (err != GL_NO_ERROR)
    {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    //select clearing (background) color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    GLboolean writeMask = false;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &writeMask);
    assert(writeMask);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    //glEnable(GL_BLEND);    // TODO: first render non alpha tiles, then alpha tiles with depth test
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Lighting makes it nice but dark
    //glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

std::map<int, Window*> Window::allWindows;

Window::Window()
{
    //Set Display Mode
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    //Set the window size
    //glutInitWindowSize(width, height);

    //Set the window position
    //glutInitWindowPosition(100,100);

        //Create the window
    handle = glutCreateWindow(nullptr);
    allWindows[handle] = this;

    Init();
    // Hook functions, TODO: this should be done once for all windows
    glutDisplayFunc(Display);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Key);
    glutSpecialFunc(SpecialKey);
}

Window::~Window()
{
        if (handle)
        {
            allWindows.erase(handle);
            glutDestroyWindow(handle);
        }
}

Window* Window::CurrentWindow()
{
    int currentHandle = glutGetWindow();
    auto it = allWindows.find(currentHandle);
    if (it == allWindows.end()) {
        return nullptr;
    }
    else
    {
        return it->second;
    }
}

void Window::Display(void)
{
    Window& window = *CurrentWindow();
    window.Render();
}

void Window::Render()
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CurrentWindow()->GetScene().Render();
    // Don't wait start processing buffered OpenGL routines
    glFlush();
}


void Window::SpecialKey(int key,  int x, int y)
{
    Debug::Log("Special Key(%d @ %d, %d)", key, x, y);
    assert(key<256); // upper byte used for ascii
    Window& window = *CurrentWindow();
    window.OnKey(key, x, y);
}

void Window::Key(unsigned char key,  int x, int y)
{
    Debug::Log("Key(%c(%d) @ %d, %d)", key, key, x, y);
    Window& window = *CurrentWindow();
    window.OnKey(std::uint16_t(key) << 8, x, y);
}

void Window::OnKey(std::uint16_t key,  int x, int y)
{
    std::uint16_t modifiers = glutGetModifiers();
    if (scene.GetCamera().Key(key, modifiers))
    {
        glutPostRedisplay();
    }
    else
    {
        app->bus.Post(KeyPressed(key, modifiers));
    }
}

void Window::Mouse(int button, int state, int x, int y)
{
    Debug::Log("Mouse(%d, %s, %d @ %d, %d, )", button, state== GLUT_UP?"up":"down", glutGetModifiers(), x,y);
    Window& window = *CurrentWindow();
    window.OnMouse(button, state, x, y);
}


Engine::Coordinate Window::Screen2View(int x, int y) const
{
    glutSetWindow(handle);
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (width<=0 || height<=0 )
    {
        return Coordinate();
    }
    return Engine::Coordinate(
        -1.0 + 2.0 * x / width,
        1.0 + -2.0 * y / height,
        0
    );
}

void Window::OnMouse(int button, int state, int x, int y)
{
    app->bus.Post(Click(button, x, y));

    auto viewPosition = Screen2View(x, y);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        auto [prop, sub ] = scene.Select(viewPosition);
        if (prop)
        {
            auto name = prop->Name();
            Engine::Debug::Log("Selected %.*s.%d", name.size(), name.data(), sub);
        }
        else
        {
            Engine::Debug::Log("Selected nothing at %f, %f", viewPosition.x, viewPosition.y);
        }
        app->bus.Post(ClickOn(prop, sub));
    }
    if (button==3 && state == GLUT_UP)
    {
        app->bus.Post(ScrollWheel(true));
        scene.GetCamera().Zoom(1.1);
        glutPostRedisplay();
    }
    else if (button==4 && state == GLUT_UP)
    {
        app->bus.Post(ScrollWheel(false));
        scene.GetCamera().Zoom(0.909090909);
        glutPostRedisplay();
    }
}


void Window::SetTitle(const char* title)
{
        glutSetWindow(handle);
        glutSetWindowTitle(title);
        glutSetIconTitle(title);
}

void Window::Fullscreen()
{
        glutSetWindow(handle);
        glutFullScreen();
}

Scene& Window::GetScene()
{
    return scene;
}


}
