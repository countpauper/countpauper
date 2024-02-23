#include <GL/glut.h>
#include "UI/Window.h"
#include "UI/Application.h"

#include "Game/Game.h"

#include "Game/Race.h"
#include "Game/Creature.h"

#include "Utility/Singleton.h"

int main(int argc, char**argv)
{
    Engine::Singleton<Engine::Application> app(argc, argv);
    Engine::Window window;
    window.SetTitle("Tactics III");

    Game::Game game(window.GetScene());
    app->Run();
}
