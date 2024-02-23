#include <GL/glut.h>
#include "UI/Window.h"
#include "UI/Application.h"
#include "File/Image.h"
#include "Geometry/Mesh.h"
#include "UI/Scene.h"
#include "Geometry/Matrix.h"
#include "Game/Map.h"
#include "Game/Creature.h"
#include "Game/Race.h"
#include "UI/Avatar.h"
#include "Utility/Singleton.h"

int main(int argc, char**argv)
{
    Engine::Singleton<Engine::Application> app(argc, argv);
    Engine::Window window;
    Game::Map map( Engine::Image("data/map20.png"));

    window.SetTitle("Tactics III");
    window.GetScene().Add(map);
    Game::Race elf("elf");
    // Todo: props are more like objects themselves and override position and orientation and render?
    // then make Mesh based objects and Avatars are those but with a link to a creature, but can be selected and such
    // objects also have mouse/keyboard input and selection as sort ModelView
    Game::Creature c("Velg'larn", elf);
    Game::Avatar a(map, c, {3, 2});
    window.GetScene().Add(a);

    Game::Creature c2("Elg'caress", elf);
    Game::Avatar a2(map, c2, {5, 8});
    window.GetScene().Add(a2);

    app->Run();
}
