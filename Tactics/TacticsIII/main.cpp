#include <cstdio>
#include <GL/glut.h>
#include "Engine/Window.h"
#include "Engine/Application.h"


int main(int argc, char**argv)
{
    Engine::Application app(argc, argv);
    Engine::Window window;

    app.Run();
}
