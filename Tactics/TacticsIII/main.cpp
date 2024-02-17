#include <GL/glut.h>
#include "Engine/Window.h"
#include "Engine/Application.h"
#include "Engine/Image.h"
#include "Game/Map.h"

int main(int argc, char**argv)
{
    Engine::Application app(argc, argv);
    Engine::Window window;
    Game::Map map( Engine::Image("data/map20.png"), 16);
    window.SetTitle("Tactics III");
    app.Run();
}
