#include <GL/glut.h>
#include "Engine/Window.h"
#include "Engine/Application.h"
#include "Engine/Image.h"
#include "Engine/Mesh.h"
#include "Engine/Scene.h"
#include "Game/Map.h"

int main(int argc, char**argv)
{
    Engine::Application app(argc, argv);
    Engine::Window window;
    Game::Map map( Engine::Image("data/map20.png"), 16);
    window.SetTitle("Tactics III");
    Engine::Box box(1.0);
    auto& prop = window.GetScene().Add(box, Engine::Coordinate());
    app.Run();
}
