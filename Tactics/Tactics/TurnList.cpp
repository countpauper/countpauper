#include "stdafx.h"
#include <gl/GL.h>
#include "Engine/Error.h"
#include "Engine/Text.h"
#include "Game/Game.h"
#include "Game/Actor.h"
#include "Game/Plan.h"
#include "TurnList.h"

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
        glPushName(actor.Id());
        RenderBackground();
        RenderActor();
        RenderPlan();
        RenderState();
        glPopName();
    }

    void TurnList::Item::RenderBackground() const
    {
        float height = static_cast<float>(Height())-2;
        if (highlighted)
            glColor3f(1.0f, 1.0f, 1.0f);
        else
            glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(128.0f-height, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glVertex2f(0.0f, height);
        glVertex2f(128.0f-height, height);
        glEnd();
    }

    void TurnList::Item::RenderActor() const
    {
        glPushMatrix();
        glTranslatef(2, 0.0f+Engine::Font::default.Height(), 0);
        Game::teamColor.at(actor.GetTeam()).Render();
        Engine::Font::default.Select();
        std::wstring text;

        if (hotKey)
        {
            wchar_t ch = MapVirtualKeyW(hotKey, MAPVK_VK_TO_CHAR);
            text = std::wstring(1, ch);
            text += L" : ";
        }
        text += actor.Description();

        Engine::glText(text);
        glPopMatrix();
    }

    void TurnList::Item::RenderPlan() const
    {
        if (!actor.plan || !actor.plan->Valid())
            return;
        glPushMatrix();
        float height = static_cast<float>(Height())-3;
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(128.0f-height, 1.0f, 0.0f);
        Engine::Image::Bind bind (actor.plan->Icon());
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex2f(0.0f, 0.0f);
            glTexCoord2f(0, 1);
            glVertex2f(0.0f, height);
            glTexCoord2f(1, 1);
            glVertex2f(height, height);
            glTexCoord2f(1, 0);
            glVertex2f(height, 0.0f);
        glEnd();
        glPopMatrix();
      
    }

    void TurnList::Item::RenderState() const
    {
        glPushMatrix();
        glTranslatef(2, 2.0f*Engine::Font::default.Height(), 0);
        Game::teamColor.at(actor.GetTeam()).Render();
        Engine::Font::default.Select();
        std::wstring text;
        if (actor.Dead())
            text = L" +";
        else if (!actor.CanAct())
            text = L" -";
        else if (actor.IsActive())
            text = L"*";
        else if (actor.IsAnticipating())
            text = L".";
        else if (actor.IsIdle())
            text = L"_";
        else
            text = L" ";
        text += L" " + std::to_wstring(actor.GetMovePoints());
        text += L" " + actor.body.Description();
        Engine::glText(text);
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

    void TurnList::Key(unsigned code)
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