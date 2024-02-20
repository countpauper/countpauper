#include <GL/glut.h>
#include "Engine/UI/Window.h"
#include "Engine/UI/Application.h"
#include "Engine/File/Image.h"
#include "Engine/Geometry/Mesh.h"
#include "Engine/UI/Scene.h"
#include "Engine/Geometry/Matrix.h"
#include "Game/Map.h"
#include "Game/Creature.h"
#include "Game/Race.h"

int main(int argc, char**argv)
{
    Engine::Application app(argc, argv);
    Engine::Window window;
    Game::Map map( Engine::Image("data/map20.png"));

    Engine::Box charMesh;
    charMesh *= Engine::Matrix::Scale({0.75, 0.75, 1.75}) * Engine::Matrix::Translation({-0.5, -0.5, 0.0});
    charMesh.SetColor(Engine::RGBA::red);

    window.SetTitle("Tactics III");
    window.GetScene().Add(Engine::Prop{&map, map.GetMesh(), Engine::Coordinate()});
    Game::Race elf("elf");
    // Todo: props are more like objects themselves and override position and orientation and render?
    // then make Mesh based objects and Avatars are those but with a link to a creature, but can be selected and such
    // objects also have mouse/keyboard input and selection as sort ModelView
    Game::Creature c("velglarn", elf);
    window.GetScene().Add(Engine::Prop{&c, charMesh, map.GroundCoord({2,3})});
    app.Run();
}
