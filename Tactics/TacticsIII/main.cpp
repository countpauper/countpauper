#include <GL/glut.h>
#include "UI/Window.h"
#include "UI/Layout.h"
#include "Game/Game.h"
#include "Game/Race.h"
#include "Game/Creature.h"
#include "File/Json.h"
#include "UI/Application.h"
#include "UI/Logging.h"
#include "Utility/Singleton.h"
#include "Utility/Assert.h"

static constexpr Engine::Logging::Level MainLogging = Engine::Logging::Warning;

int main(int argc, char**argv)
{
    try
    {
        Engine::Singleton<Engine::Application> app(argc, argv);
        Engine::Window window;
        window.SetTitle("Tactics III");
        Game::UI::Layout layout;
        window.GetHUD().Add(layout.root());
        Game::Game game(window.GetScene(), Engine::LoadJson("level.json"));
        app->Run();
        Engine::SaveJson(game.Serialize(), "savegame.json");
    }
    catch(const std::exception& ex)
    {
        Engine::Logging::Log<MainLogging, Engine::Logging::Fatal>("Exception: %s", ex.what());
        return 1;
    }
    return 0;
}
