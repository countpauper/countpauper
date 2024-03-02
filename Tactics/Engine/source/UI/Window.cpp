#include "UI/Window.h"
#include "UI/Debug.h"
#include "Logging.h"
#include "Rendering/Color.h"
#include "UI/WindowMessages.h"
#include "Utility/Singleton.h"
#include "UI/Application.h"
#include "Rendering/Text.h"
#include "Geometry/Coordinate.h"

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
    Application::Get().bus.Subscribe(*this, {
            MessageType<Redraw>()
        });
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
    glutUseLayer(GLUT_NORMAL);
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CurrentWindow()->GetScene().Render();
    CurrentWindow()->GetHUD().Render();
/*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(2.0, -2.0, 1.0);       // make positive y go down
    glTranslated(-0.5, -0.5, -1.0); // top left and front of z-buffer

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glText("User\ninterface");
    // Don't wait start processing buffered OpenGL routines
*/
    glFlush();
}

void Window::OnMessage(const Engine::Message& message)
{
    if (auto redraw = message.Cast<Redraw>())
    {
        glutPostRedisplay();
        glutPostOverlayRedisplay();
    }
}

void Window::SpecialKey(int key,  int x, int y)
{
    Logging::Log<UiLogging, Logging::Info>("Special Key(%d @ %d, %d)", key, x, y);
    assert(key<256); // upper byte used for ascii
    Window& window = *CurrentWindow();
    window.OnKey(key, 0, x, y);
}

void Window::Key(unsigned char key,  int x, int y)
{
    Logging::Log<UiLogging, Logging::Info>("Key(%c(%d) @ %d, %d)", key, key, x, y);
    Window& window = *CurrentWindow();
    window.OnKey(0, key, x, y);
}

void Window::OnKey(std::uint8_t code, unsigned char ascii, int x, int y)
{
    std::uint16_t modifiers = glutGetModifiers();
    app->bus.Post(KeyPressed(code, ascii, modifiers));
}

void Window::Mouse(int button, int state, int x, int y)
{
    Logging::Log<UiLogging, Logging::Info>("Mouse(%d, %s, %d @ %d, %d, )", button, state== GLUT_UP?"up":"down", glutGetModifiers(), x,y);
    Window& window = *CurrentWindow();
    window.OnMouse(button, state, x, y);
}

Engine::Vector Window::PixelScale() const
{
    glutSetWindow(handle);
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    return Vector{
        width>=0 ? 1.0 / width : 0.0 ,
        height>=0 ? 1.0 / height : 0.0,
         0};
}

Coordinate Window::Screen2View(int x, int y) const
{
    glutSetWindow(handle);
    Vector pixel = PixelScale();
    return Coordinate(
        -1.0 + 2.0 * x * pixel.x,
        1.0 + -2.0 * y * pixel.y,
        0 * pixel.z
    );
}

void Window::OnMouse(int button, int state, int x, int y)
{
    app->bus.Post(Click(button, x, y));

    auto viewPosition = Screen2View(x, y);
    auto control = hud.Click(viewPosition);

    if (control)
        return; // TODO bus it ? for now at least don't check the scene below it

    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        auto [prop, sub ] = scene.Select(viewPosition);
        if (prop)
        {
            auto name = prop->Name();
            Logging::Log<UiLogging, Logging::Info>("Selected %.*s.%d", name.size(), name.data(), sub);
        }
        else
        {
            Logging::Log<UiLogging, Logging::Info>("Selected nothing at %f, %f", viewPosition.x, viewPosition.y);
        }
        app->bus.Post(ClickOn(prop, sub));
    }
    // TODO bus
    if (button==3 && state == GLUT_UP)
    {
        app->bus.Post(ScrollWheel(true));

    }
    else if (button==4 && state == GLUT_UP)
    {
        app->bus.Post(ScrollWheel(false));
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

HUD& Window::GetHUD()
{
    return hud;
}



}
