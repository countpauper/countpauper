#include "stdafx.h"
#include "Game.h"

namespace Game
{

    Game::Game() = default;

    std::wistream& operator>>(std::wistream& s, Game& game)
    {
        game.m_map = std::make_unique<Map>();
        s >> *game.m_map;
        return s;
    }

}   // ::Game


