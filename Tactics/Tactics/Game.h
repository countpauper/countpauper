#pragma once

#include <memory>
#include "Map.h"
#include <istream>

namespace Game
{
    class Game
    {
    public:
        Game();
        void Render();
    protected:
        friend std::wistream& operator>>(std::wistream& s, Game& game);
        std::unique_ptr<Map> m_map;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

