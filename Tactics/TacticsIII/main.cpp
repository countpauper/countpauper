#include <GL/glut.h>
#include "Engine/Window.h"
#include "Engine/Application.h"
#include "Engine/Image.h"
#include "Engine/Mesh.h"
#include "Engine/Scene.h"
#include "Engine/Matrix.h"
#include "Game/Map.h"

int main(int argc, char**argv)
{
    Engine::Application app(argc, argv);
    Engine::Window window;
    Game::Map map( Engine::Image("data/map20.png"), 16);
    Engine::Box charMesh;
    charMesh *= Engine::Matrix::Scale({0.75, 0.75, 1.75}) * Engine::Matrix::Translation({-0.5, -0.5, 0.0});
    charMesh.SetColor(Engine::RGBA::red);
    window.SetTitle("Tactics III");
    auto& mapProp = window.GetScene().Add("map", map.GetMesh(), Engine::Coordinate());
    auto& p1 = window.GetScene().Add("p1", charMesh, map.GroundCoord({2,3}));
    app.Run();
}
