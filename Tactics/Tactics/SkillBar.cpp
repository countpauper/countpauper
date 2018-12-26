#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Error.h"
#include "SkillBar.h"
#include "Game.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

    
    SkillBar::Button::Button(const Skill* skill, unsigned hotKey) :
        hotKey(hotKey),
        highlighted(false),
        skill(skill)
    {
        if (skill)
        {
            texture.Load(std::wstring(L"Data/") + skill->name + std::wstring(L".png"));
        }
        else
        {
            texture.Load(L"Data/Wait.png");
        }
    }

    SkillBar::Button::Button(Button&& other) :
        hotKey(other.hotKey),
        skill(other.skill),
        texture(std::move(other.texture)),
        highlighted(other.highlighted)
    {

    }

    void SkillBar::Button::Highlight(bool on)
    {
        highlighted = on;
    }

    unsigned SkillBar::Button::HotKey() const
    {
        return hotKey;
    }

    void SkillBar::Button::Render() const
    {
        if (skill)
            glPushName(GLuint(skill->Id()));
        else
            glPushName(0);

        texture.Bind();
        if (highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.25f, 0.25f, 0.25f);
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
        texture.Unbind();
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
        for (auto& button : buttons)
        {
            if (button.HotKey() == code)
            {
                game.SelectSkill(button.skill);
            }
        }
    }

    void SkillBar::UpdateSkills(const Actor* newActor)
    {
        actor = newActor;
        buttons.clear();
        if (actor)
        {
            buttons.emplace_back(Button(nullptr, VK_ESCAPE));
            auto hotKey = VK_F1;
            for (auto skill : actor->GetSkills())
            {
                if (skill.skill->IsActive())
                {
                    buttons.emplace_back(Button(skill.skill, hotKey++));
                }
                if (skill.skill == game.SelectedSkill())
                {
                    buttons.back().Highlight(true);
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