#pragma once

#include <vector>
#include "Image.h"

namespace Game
{
    class Game;


    class Button
    {
    public:
        Button(const std::wstring& name);
        Button(const Button&) = delete;
        Button(Button&& other);
        void Render() const;
        void Highlight();
    private:
        Engine::Image m_texture;
        bool m_highlighted;
    };
    class Panel
    {
    public:
        Panel(Game& game, unsigned height);
        void Render() const;
        unsigned Height() const;
    private:
        void Initialize(const Actor* actor);
        Game& m_game;
        std::list<Button> m_buttons;
        unsigned m_height;
        const Actor* m_actor;
    };
}