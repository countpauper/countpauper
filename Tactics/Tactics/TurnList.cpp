#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Error.h"
#include "Engine/Text.h"
#include "Game.h"
#include "TurnList.h"
#include "Actor.h"

namespace Game
{

    TurnList::Item::Item(const Actor& actor) :
        highlighted(false),
        actor(actor)
    {
    }

    TurnList::Item::Item(TurnList::Item&& other) :
        actor(other.actor),
        highlighted(other.highlighted)
    {

    }

    void TurnList::Item::Highlight(bool on)
    {
        highlighted = on;
    }
    void TurnList::Item::Render() const
    {
        glPushName(actor.Id());
        if (highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(100.0f, -15.0f);
            glVertex2f(0.0f, -15.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(100.0f, 0.0f);
        glEnd();
        glColor3f(1, 1, 1);
        Engine::Font::system.Select();
        Engine::glText(actor.Description());
        glPopName();
    }

    TurnList::TurnList(Game& game, unsigned width) :
        game(game),
        width(width),
        actorConnection(game.actorActivated.connect([this](Actor* actor)
    {
        Update();
    }))
    {
    }

    unsigned TurnList::Width() const
    {
        return width;
    }

    void TurnList::Render() const
    {
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_PROJECTION);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushName(GLuint(Game::Selection::Object));
        glColor3f(1, 1, 1);
        for (auto& item : items)
        {
            glTranslatef(0.0f, 16.0f, 0.0f);  // TODO: system font height
            item.Render();
        }
        glPopName();
        Engine::CheckGLError();
    }

    void TurnList::Key(unsigned short code)
    {
    }

    void TurnList::Update()
    {
        items.clear();
        for (auto& objPtr : game.GetObjects())
        {
            const Actor* actor = dynamic_cast<Actor*>(objPtr.get());
            if (actor)
            {
                items.emplace_back(Item(*actor));
            }
        }
    }

};