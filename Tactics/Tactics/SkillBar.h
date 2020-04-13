#pragma once

#include <vector>
#include <connection.h>
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
        void Key(unsigned code);

        class Button
        {
        public:
            Button(const Skill *skill, unsigned hotKey);
            Button(const Button&) = delete;
            Button(Button&& other);
            void Render() const;
            unsigned HotKey() const;
            void Enable(bool on);
            void Highlight(bool on);
            const Skill* skill;

        private:
            Engine::Image icon;
            unsigned hotKey;
            bool enabled;
            bool highlighted;
        };

    private:
        void UpdateSkills(const Actor* actor);
        void HighlightSkill(const Skill* skill);
        Game& game;
        std::vector<Button> buttons;
        unsigned height;
        const Actor* actor;
        is::signals::scoped_connection actorConnection;
        is::signals::scoped_connection skillConnection;
    };
}