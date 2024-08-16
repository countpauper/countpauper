#include <GL/glut.h>
#include "UI/Window.h"
#include "UI/Application.h"
#include "UI/Logging.h"
#include "Game/Game.h"
#include "Game/Race.h"
#include "Game/Creature.h"
#include "File/Json.h"
#include "UI/Label.h"
#include "UI/Splitter.h"
#include "Utility/Singleton.h"

static constexpr Engine::Logging::Level MainLogging = Engine::Logging::Warning;

int main(int argc, char**argv)
{
    try
    {
        Engine::Singleton<Engine::Application> app(argc, argv);
        Engine::Window window;

        window.SetTitle("Tactics III");
        Engine::Label lbl("left_lbl", "Select an avatar");
        Engine::Label lbl2("right_lbl", "Make a plan");
        Engine::Splitter splitter(lbl, lbl2);
        Engine::Label lbl3("log_lbl", "");
        lbl3.vertical_align = Engine::Align::bottom;
        Engine::Splitter splitter2(splitter, lbl3, 0.75, false);
        window.GetHUD().Add(splitter2);
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
