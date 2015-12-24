#include "stdafx.h"
#include "Game.h"

namespace Game
{

    Game::Game() :
        player(0)
    {
    }

    void Game::Render() const
    {
        map->Render();
        for (const auto& object: objects)
            object->Render();
    }

    void Game::Key(unsigned short code)
    {
        auto& playerObject = *objects.at(player);
        switch (code)
        {
        case VK_LEFT:
            playerObject.Move(-1, 0);
            break;
        case VK_RIGHT:
            playerObject.Move(1, 0);
            break;
        case VK_UP:
            playerObject.Move(0, 1);
            break;
        case VK_DOWN:
            playerObject.Move(0, -1);
            break;
        }
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


