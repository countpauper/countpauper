#include "stdafx.h"
#include <gl/GL.h>
#include <deque>
#include "Engine/Error.h"
#include "SkillBar.h"
#include "Game/Game.h"
#include "Game/Actor.h"
#include "Game/Skills.h"

namespace Game
{
    SkillBar::Button::Button(const Skill* skill, unsigned hotKey) :
        hotKey(hotKey),
        enabled(false),
        highlighted(false),
        skill(skill)
    {
        if (skill)
        {
            icon = skill->Icon();
        }
        else
        {
            icon.Load(L"Data/Wait.png");
        }
    }

    SkillBar::Button::Button(Button&& other) :
        hotKey(other.hotKey),
        skill(other.skill),
        icon(std::move(other.icon)),
        enabled(other.enabled),
        highlighted(other.highlighted)
    {
    }

    void SkillBar::Button::Enable(bool on)
    {
        enabled = on;
    }

    void SkillBar::Button::Highlight(bool on)
    {
        highlighted = on;
    }

    unsigned SkillBar::Button::HotKey() const
    {
        if (enabled)
            return hotKey;
        else
            return 0;
    }

    void SkillBar::Button::Render() const
    {
        if ((skill) && (enabled))
            glPushName(GLuint(skill->Id()));
        else
            glPushName(0);

        Engine::Image::Bind bind(icon);
        if (highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else if (enabled)
            glColor3f(0.6f, 0.6f, 0.6f);
        else
            glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(1.0f, 0.0f);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(1.0f, 1.0f);

            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0.0f, 1.0f);
        glEnd();
        glPopName();
    }

    SkillBar::SkillBar(Game& game, unsigned height) :
        game(game),
        height(height),
        actor(nullptr),
        actorConnection(game.actorSelected.connect([this](const Actor* actor)
        {
            UpdateSkills(actor);
        })),
        skillConnection(game.skillSelected.connect([this](const Skill* skill)
        {
            HighlightSkill(skill);
        }))
    {
    }

    unsigned SkillBar::Height() const
    {
        return height;
    }

    void SkillBar::Render() const
    {
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glOrtho(0, viewport[2], 0, viewport[3], -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(float(height), float(height), 1.0f);

        glPushName(GLuint(Game::Selection::Skill));
        for (auto& button : buttons)
        {
            button.Render();
            glTranslatef(1.0f, 0.0f, 0.0f);
        }
        glPopName();
        Engine::CheckGLError();
    }

    void SkillBar::Key(unsigned short code)
    {
        Button * selectedButton = nullptr;
        for (auto& button : buttons)
        {
            if (button.HotKey() == code)
            {
                selectedButton = &button;
            }
        }
        if (selectedButton)
        {
            game.SelectSkill(selectedButton->skill);
        }
    }

    void SkillBar::UpdateSkills(const Actor* newActor)
    {
        actor = newActor;
        buttons.clear();
        if (actor)
        {
            std::deque<unsigned> hotkeys = {
                VK_ESCAPE, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12
            };
            for (auto skill : actor->GetSkills())
            {
                if (skill->IsActive())
                {
                    buttons.emplace_back(Button(skill, hotkeys.front()));
                    hotkeys.pop_front();
                    buttons.back().Enable(actor->IsPossible(*skill));
                    if (skill == game.SelectedSkill())
                    {
                        buttons.back().Highlight(true);
                    }
                }
            }
        }
    }

    void SkillBar::HighlightSkill(const Skill* skill)
    {
        for (auto& button : buttons)
        {
            if (button.skill == skill)
            {
                button.Highlight(true);
            }
            else
            {
                button.Highlight(false);
            }
        }
    }
};