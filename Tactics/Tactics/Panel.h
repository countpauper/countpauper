#pragma once

#include <vector>
#include "Image.h"

namespace Game
{
    class Game;


    class Button
    {
    public:
        Button(const std::string& name);
        Button(const Button&) = delete;
        Button(Button&& other);
        void Render() const;
    private:
        Engine::Image m_texture;
    };
    class Panel
    {
    public:
        Panel(Game& game, unsigned height);
        void Render() const;
        unsigned Height() const;
    private:
        Game& m_game;
        std::vector<Button> m_buttons;
        unsigned m_height;
    };
}