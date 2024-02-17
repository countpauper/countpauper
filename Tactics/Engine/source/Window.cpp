#include "Engine/Window.h"
#include "Engine/Debug.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <assert.h>

namespace Engine
{

void init(void)
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

    //Call init (initialise GLUT
    init();

    //Call "display" function
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
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    CurrentWindow()->GetScene().Render();
/*
    //draw white polygon (rectangle) with corners at
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_POLYGON);
        glVertex3f(0.25, 0.25, 0.0);
        glVertex3f(0.75, 0.25, 0.0);
        glVertex3f(0.75, 0.75, 0.0);
        glVertex3f(0.25, 0.60, 0.0);
    glEnd();
*/

    // Don't wait start processing buffered OpenGL routines
    glFlush();
}


void Window::SpecialKey(int key,  int x, int y)
{
    Debug::Log("Special Key(%d @ %d, %d)", key, x, y);
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
    if (window.GetScene().GetCamera().Key(key, glutGetModifiers()))
    {
        glutPostRedisplay();
    }
}

void Window::Mouse(int button, int state, int x, int y)
{
    Debug::Log("Mouse(%d, %s, %d @ %d, %d, )", button, state== GLUT_UP?"up":"down", glutGetModifiers(), x,y);
    Window& window = *CurrentWindow();
    if (button==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        window.GetScene().GetCamera().Drag(x, y);
    }
    else if (button==GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // Way too fast window.GetScene().GetCamera().FinishDrag(x, y);
        // glutPostRedisplay();
    }
    else if (button==3 && state == GLUT_UP)
    {
        window.GetScene().GetCamera().Zoom(0.909090909);
        glutPostRedisplay();
    }
    else if (button==4 && state == GLUT_UP)
    {
        window.GetScene().GetCamera().Zoom(1.1);
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
