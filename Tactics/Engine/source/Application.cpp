#include "Engine/Application.h"
#include <GL/glut.h>

namespace Engine
{

Application::Application(int argc, char** argv)
{
    glutInit(&argc, argv);
}


void Application::Run()
{
    glutMainLoop();

}
}