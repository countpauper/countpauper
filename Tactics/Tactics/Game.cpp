#include "stdafx.h"
#include "Game.h"

namespace Game
{

    Game::Game() = default;

    void Game::Render() const
    {
        map->Render();
        for (const auto& object: objects)
            object->Render();
    }

    std::wistream& operator>>(std::wistream& s, Game& game)
    {
        unsigned objects;
        s >> objects;
        for (unsigned oi = 0; oi < objects; ++oi)
        {
            std::wstring type;
            s >> type;
            if (type == L"Actor")
            {
                auto actor = std::make_unique < Actor>();
                s >> *actor;
                game.objects.emplace_back(std::move(actor));

            }
        }
        game.map = std::make_unique<Map>();
        s >> *game.map;
        return s;
    }

}   // ::Game


