#include "UI/Application.h"
#include "UI/WindowMessages.h"
#include "Utility/Singleton.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Engine
{

Application::Application(int argc, char** argv)
{
    glutInit(&argc, argv);
    bus.Subscribe(*this, {MessageType<KeyPressed>()});
}

Application& Application::Get()
{
    return Singleton<Application>::Get();
}


void Application::Run()
{
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutMainLoop();
}


void Application::OnMessage(const Message& message)
{
    if (auto key = dynamic_cast<const KeyPressed*>(&message))
    {
        if (key->ascii == 27)
        {
            glutLeaveMainLoop();
        }
    }
}

}
