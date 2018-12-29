#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Error.h"
#include "Engine/Text.h"
#include "Game.h"
#include "TurnList.h"
#include "Actor.h"
#include "Plan.h"

namespace Game
{

    TurnList::Item::Item(const Actor& actor, unsigned hotKey) :
        highlighted(false),
        hotKey(hotKey),
        actor(actor)
    {
    }

    TurnList::Item::Item(TurnList::Item&& other) :
        actor(other.actor),
        highlighted(other.highlighted),
        hotKey(other.hotKey)
    {

    }

    void TurnList::Item::Highlight(bool on)
    {
        highlighted = on;
    }

    unsigned TurnList::Item::Height() const
    {
        return 3 + 2 * Engine::Font::default.Height();
    }

    unsigned TurnList::Item::HotKey() const
    {
        return hotKey;
    }

    void TurnList::Item::Render() const
    {
        float height = static_cast<float>(Height());
        glPushName(actor.Id());
        // background
        if (highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(128.0f, 1.0f);
            glVertex2f(0.0f, 1.0f);
            glVertex2f(0.0f, height);
            glVertex2f(128.0f, height);
        glEnd();
        glPushMatrix();
        glTranslatef(2, Engine::Font::default.Height()+1.0f, 0); 
        Game::teamColor.at(actor.GetTeam()).Render();
        Engine::Font::default.Select();
        auto text = actor.Description();
        
        if (actor.Dead())
            text += L"+";
        else if (!actor.CanAct())
            text += L"-";
        if (hotKey)
        {
            wchar_t ch = MapVirtualKeyW(hotKey, MAPVK_VK_TO_CHAR);
            text += L" : ";
            text += std::wstring(1, ch);
        }
        Engine::glText(text);
        glTranslatef(2, Engine::Font::default.Height() + 1.0f, 0);

        text = std::to_wstring(actor.GetMovePoints());
        if (actor.plan)
        {
            text += L" ";
            text += actor.plan->Description();
        }
        Engine::glText(text);
        glPopName();

        glPopMatrix();
    }

    TurnList::TurnList(Game& game, unsigned width) :
        game(game),
        width(width),
        selectedConnection(game.actorSelected.connect([this](const Actor*)
        {
            Update();
        })),
        activatedConnection(game.actorsActivated.connect([this](const Game::ActorList&)
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
            item.Render();
            glTranslatef(0.0f, static_cast<float>(item.Height()), 0.0f);
        }
        glPopName();
        Engine::CheckGLError();
    }

    void TurnList::Key(unsigned short code)
    {
        if (code == VK_OEM_3)   // `
            game.SelectTarget(nullptr);
        else
        {
            const Actor* selectedActor = nullptr;
            for (auto& item : items)
            {
                if (item.HotKey() == code)
                {
                    selectedActor = &item.actor;
                }
            }
            if (selectedActor)
                game.SelectTarget(selectedActor);
        }
    }

    void TurnList::Update()
    {
        items.clear();
        unsigned hotkey = '0';
        for (auto& objPtr : game.GetObjects())
        {
            const Actor* actor = dynamic_cast<Actor*>(objPtr.get());
            if (actor)
            {
                items.emplace_back(Item(*actor, hotkey++));
            }
            items.back().Highlight(actor == game.SelectedActor());
        }
    }

};