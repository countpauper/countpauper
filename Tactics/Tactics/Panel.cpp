#include "stdafx.h"
#include <gl/GL.h>
#include "Error.h"
#include "Game.h"
#include "Panel.h"

namespace Game
{

    
    Button::Button(const std::string& name)
    {
        m_texture.Load(std::string("Data/") + name + std::string(".png"));
    }

    Button::Button(Button&& other) :
        m_texture(std::move(other.m_texture))
    {

    }

    void Button::Render() const
    {
//        m_texture.Load(std::string("Data/Sword.png"));
        m_texture.Bind();
        glColor3f(1.0f, 1.0f, 1.0f);
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
        m_height(height)
    {
        m_buttons.emplace_back(Button("Sword"));
        m_buttons.emplace_back(Button("Bow"));
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
        for (auto& button : m_buttons)
        {
            button.Render();
            glTranslatef(1.0f, 0.0f, 0.0f);
        }
        Engine::CheckGLError();

    }
};