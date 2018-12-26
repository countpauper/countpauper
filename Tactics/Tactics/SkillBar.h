#pragma once

#include <vector>
#include <boost/signals2.hpp>
#include "Engine/Image.h"

namespace Game
{
    class Game;
    class Skill;
    class Actor;

    class SkillBar
    {
    public:
        SkillBar(Game& game, unsigned height);
        void Render() const;
        unsigned Height() const;
        void Key(unsigned short code);

        class Button
        {
        public:
            Button(const Skill *skill, unsigned hotKey);
            Button(const Button&) = delete;
            Button(Button&& other);
            void Render() const;
            unsigned HotKey() const;
            void Highlight(bool on);
            const Skill* skill;

        private:
            Engine::Image texture;
            unsigned hotKey;
            bool highlighted;
        };

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