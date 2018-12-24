#pragma once

#include <vector>
#include "Engine/Image.h"

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
        void Highlight(bool on);
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
        void UpdateSkills(Actor* actor);
        void HighlightSkill(const Skill* skill);
        Game& game;
        std::vector<Button> buttons;
        unsigned height;
        Actor* actor;
        boost::signals2::scoped_connection actorConnection;
        boost::signals2::scoped_connection skillConnection;
    };
}