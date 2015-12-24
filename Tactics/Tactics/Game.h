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
    protected:
        friend std::wistream& operator>>(std::wistream& s, Game& game);
        std::unique_ptr<Map> map;
        std::vector<std::unique_ptr<Object>> objects;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

