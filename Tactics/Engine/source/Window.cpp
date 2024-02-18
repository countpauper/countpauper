#include "Engine/Window.h"
#include "Engine/Debug.h"
#include "Engine/Color.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <assert.h>

namespace Engine
{

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
    if (window.GetScene().GetCamera().Key(key, glutGetModifiers()))
    {
        glutPostRedisplay();
    }
}

void Window::Key(unsigned char key,  int x, int y)
{
    Debug::Log("Key(%c(%d) @ %d, %d)", key, key, x, y);
    Window& window = *CurrentWindow();
    window.OnKey(std::uint16_t(key) << 8, x, y);
}

void Window::OnKey(std::uint16_t  key,  int x, int y)
{
    if (scene.GetCamera().Key(key, glutGetModifiers()))
    {
        glutPostRedisplay();
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
    auto viewPosition = Screen2View(x, y);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        auto [prop, name ] = scene.Select(viewPosition);
        if (prop)
        {
            Engine::Debug::Log("Selected %.*s.%d", prop->Name().size(), prop->Name().data(), name);
        }
        else
        {
            Engine::Debug::Log("Selected nothing at %f, %f", viewPosition.x, viewPosition.y);
        }
    }
    if (button==3 && state == GLUT_UP)
    {
        scene.GetCamera().Zoom(0.909090909);
        glutPostRedisplay();
    }
    else if (button==4 && state == GLUT_UP)
    {
        scene.GetCamera().Zoom(1.1);
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