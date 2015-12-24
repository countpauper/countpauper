#pragma once

#include <memory>
#include <istream>
#include "Map.h"
#include "Actor.h"

namespace Game
{
    class Game
    {
    public:
        Game();
        void Render() const;
        void Key(unsigned short code);
    protected:
        friend std::wistream& operator>>(std::wistream& s, Game& game);
        std::unique_ptr<Map> map;
        std::vector<std::unique_ptr<Object>> objects;
        unsigned player;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

