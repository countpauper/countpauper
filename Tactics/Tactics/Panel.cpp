#include "stdafx.h"
#include <gl/GL.h>
#include "Error.h"
#include "Game.h"
#include "Panel.h"
#include "Actor.h"
#include "Skills.h"

namespace Game
{

    
    Button::Button(const Skill& skill) :
        highlighted(false),
        skill(skill)
    {
        texture.Load(std::wstring(L"Data/") + skill.name + std::wstring(L".png"));
    }

    Button::Button(Button&& other) :
        skill(other.skill),
        texture(std::move(other.texture)),
        highlighted(other.highlighted)
    {

    }

    void Button::Highlight()
    {
        highlighted = true;
    }
    void Button::Render() const
    {
//        texture.Load(std::string("Data/Sword.png"));
        texture.Bind();
        if (highlighted)
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
        texture.Unbind();
    }

    Panel::Panel(Game& game, unsigned height) :
        game(game),
        height(height),
        actor(nullptr)
    {
    }

    unsigned Panel::Height() const
    {
        return height;
    }

    void Panel::Render() const
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(float(height), float(height), 1.0f);

        Actor* activeActor = game.ActiveActor();
        if (actor != activeActor)
            const_cast<Panel*>(this)->Initialize(activeActor);    // TODO: get change from game
        for (auto& button : buttons)
        {
            button.Render();
            glTranslatef(1.0f, 0.0f, 0.0f);
        }
        Engine::CheckGLError();
    }

    void Panel::Key(unsigned short code)
    {
        if ((code >= VK_F1) && (code <= VK_F24))
        {
            unsigned button = code - VK_F1;
            if ((actor) && (button < buttons.size()))
            {
                game.MakePlan(*actor, buttons.at(button).skill);
            }
        }
        else
            game.Key(code);
    }

    void Panel::Initialize(Actor* newActor)
    {
        actor = newActor;
        buttons.clear();
        if (actor)
        {
            for (auto skill : actor->GetSkills())
            {
                buttons.emplace_back(Button(*skill.skill));
            }
            buttons.front().Highlight();
        }
    }
};