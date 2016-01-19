#include "stdafx.h"
#include <gl/GL.h>
#include "Error.h"
#include "Game.h"
#include "Panel.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

    
    Button::Button(const std::wstring& name) :
        m_highlighted(false)
    {
        m_texture.Load(std::wstring(L"Data/") + name + std::wstring(L".png"));
    }

    Button::Button(Button&& other) :
        m_texture(std::move(other.m_texture)),
        m_highlighted(other.m_highlighted)
    {

    }

    void Button::Highlight()
    {
        m_highlighted = true;
    }
    void Button::Render() const
    {
//        m_texture.Load(std::string("Data/Sword.png"));
        m_texture.Bind();
        if (m_highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(1.0f, 0.0f);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0.0f, 1.0f);
        glEnd();
        m_texture.Unbind();

    }
    Panel::Panel(Game& game, unsigned height) :
        m_game(game),
        m_height(height),
        m_actor(nullptr)
    {
    }

    unsigned Panel::Height() const
    {
        return m_height;
    }

    void Panel::Render() const
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(float(m_height), float(m_height), 1.0f);

        Actor* actor = m_game.ActiveActor();
        if (actor != m_actor)
            const_cast<Panel*>(this)->Initialize(actor);    // TODO: get change from game
        for (auto& button : m_buttons)
        {
            button.Render();
            glTranslatef(1.0f, 0.0f, 0.0f);
        }
        Engine::CheckGLError();
    }

    void Panel::Initialize(const Actor* actor)
    {
        m_actor = actor;
        m_buttons.clear();
        if (m_actor)
        {
            for (auto skill : m_actor->GetSkills())
            {
                m_buttons.emplace_back(Button(skill.skill->name));
            }
            m_buttons.front().Highlight();
        }
    }
};