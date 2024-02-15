#include "Engine/Window.h"
#include "Engine/Debug.h"
#include <GL/glut.h>

namespace Engine
{

void init(void)
{
    //select clearing (background) color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //initialize viewing values
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}


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
    //Call init (initialise GLUT
    init();

    //Call "display" function
    glutDisplayFunc(Display);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Key);
}

Window::~Window()
{
        if (handle)
        {
                glutDestroyWindow(handle);
        }
}

void Window::Display(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT);

    //draw white polygon (rectangle) with corners at
    // (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_POLYGON);
        glVertex3f(0.25, 0.25, 0.0);
        glVertex3f(0.75, 0.25, 0.0);
        glVertex3f(0.75, 0.75, 0.0);
        glVertex3f(0.25, 0.60, 0.0);
    glEnd();

    // Don't wait start processing buffered OpenGL routines
    glFlush();
}

void Window::Key(unsigned char key,  int x, int y)
{
    Debug::Log("Key(%d @ %d, %d)", key, x, y);
}

void Window::Mouse(int button, int state, int x, int y)
{
    Debug::Log("Mouse(%d, %s, %d @ %d, %d, )", button, state== GLUT_UP?"up":"down", glutGetModifiers(), x,y);
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



}
