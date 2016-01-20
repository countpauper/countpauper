#pragma once

#include <vector>
#include "Image.h"

namespace Game
{
    class Game;
    class Skill;

    class Button
    {
    public:
        Button(const Skill& skill);
        Button(const Button&) = delete;
        Button(Button&& other);
        void Render() const;
        void Highlight();
        const Skill& skill;
    private:
        Engine::Image texture;
        bool highlighted;
    };

    class Panel
    {
    public:
        Panel(Game& game, unsigned height);
        void Render() const;
        unsigned Height() const;
        void Key(unsigned short code);
    private:
        void Initialize(Actor* actor);
        Game& game;
        std::vector<Button> buttons;
        unsigned height;
        Actor* actor;
    };
}