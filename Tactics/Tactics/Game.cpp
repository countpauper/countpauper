#include "stdafx.h"
#include <gl/GL.h>
#include "Game.h"
#include "Map.h"
#include "Actor.h"
#include "Move.h"
#include "Plan.h"

namespace Game
{

    Game::Game() :
        player(0)
    {
    }
    
    Game::~Game() = default;

    void Game::Render() const
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushName(GLuint(Selection::Map));
        map.Render();
        glPopName();
        glPushName(GLuint(Selection::Object));
        unsigned index = 0;
        for (const auto& object : objects)
        {
            glPushMatrix();
            auto position = object->GetPosition();
            auto square = map.At(position);

            glTranslatef(float(position.x)+0.5f, square.Z(), float(position.y)+0.5f);
            glPushName(index++);
            object->Render();
            glPopName();
            glPopMatrix();
        }
        glPopName();
        if (plan)
            plan->Render();
    }

    void Game::Key(unsigned short code)
    {
        if (code == VK_ESCAPE)
        {
            plan.reset();
            return;
        }
        auto& playerActor = *dynamic_cast<Actor*>(objects.at(player).get());
        if (code == VK_RETURN)
        {
            if (plan)
                plan->Execute(playerActor);
            playerActor.Turn();
            plan.reset();
            return;
        }
        if (Action::keymap.count(code) == 0)
            return;

        if (!plan)
            plan = std::make_unique<Plan>(playerActor);
        std::unique_ptr<Action> action(Action::keymap[code](plan->Final(), *this));
        if (!action)
            return;
        State state(plan->Final());
        auto result = action->Act(state, *this);
        if (!result.possible)
            return;
        plan->Add(std::move(action), result);
    }

    bool Game::CanBe(const Position& position) const
    {
        if (!map.CanBe(position))
            return false;
        for (const auto& object : objects)
            if (object->GetPosition() == position)
                return false;
        return true;
    }

    bool Game::CanGo(const Position& from, Direction direction) const
    {
        return map.CanGo(from, direction);
    }

    Actor* Game::FindTarget(const Position& from) const
    {
        for (auto& object : objects)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            if (!actor)
                continue;
            if (actor->GetPosition().Distance(from) == 1)    //TODO: more explicitly exclude actor, by team 
                return actor;
        }
        return nullptr;
    }
    std::wistream& operator>>(std::wistream& s, Game& game)
    {
        unsigned objects;
        s >> objects;
        for (unsigned oi = 0; oi < objects; ++oi)
        {
            std::wstring type;
            s >> type;
            if (type == L"Actor")
            {
                auto actor = std::make_unique < Actor>();
                s >> *actor;
                game.objects.emplace_back(std::move(actor));

            }
        }
        s >> game.map;
        return s;
    }

    void Game::Click(Selection selection, uint32_t value)
    {
        if (selection == Selection::Map)
        {
            auto& playerActor = *dynamic_cast<Actor*>(objects.at(player).get());
            Position target(value & 0xFFFF, (value >> 16) & 0xFFFF);
            plan.reset(PathPlan(playerActor, target));
        }
    }

    Plan* Game::PathPlan(const Actor& actor,const Position& target) const
    {
        std::vector<std::function<Action*(void)>> actions({
            [](){ return new North();  },
            [](){ return new East();  },
            [](){ return new South();  },
            [](){ return new West();  },
        });
        struct Node
        {
            Node(const State& state) :
                previous(nullptr),
                state(state)
            {
            }
            Node(Node& previous, Action* action, const State& state) :
                previous(&previous),
                action(action),
                state(state)
            {
            }

            
            Node* previous;
            std::unique_ptr<Action> action;
            State state;
            int Score(const Position& target, unsigned startMovePoints) const
            {
                return target.Distance(state.position) + 
                    startMovePoints - state.mp;
            }
            bool Reached(const Position& target) const
            {
                return target == state.position;
            }
        };
        typedef std::multimap<int, std::unique_ptr<Node>> OpenTree;
        OpenTree open;
        typedef std::vector<std::unique_ptr<Node>> ClosedList;
        ClosedList closed;
        State start(actor);
        auto first = std::make_unique<Node>(start);
        if (first->Reached(target))
            return nullptr;
        open.insert(std::make_pair(first->Score(target, start.mp), std::move(first)));
        while (!open.empty())
        {
            auto best = std::move(open.begin()->second);
            open.erase(open.begin());
            Node& bestNode = *best;
            closed.emplace_back(std::move(best));
            for (const auto& actionFactory : actions)
            {
                auto action = actionFactory();
                auto newState = action->Act(bestNode.state, *this);
                if (!newState.possible)
                    continue;
                bool alreadyClosed = false;
                // TODO: make function
                for (const auto& closedNode : closed)
                    if (closedNode->state.position == newState.position)
                    {
                        alreadyClosed = true;
                        break;
                    }
                if (alreadyClosed)    // TODO if new score < closed, still allow? is this possible?
                    continue;
                auto newNode = new Node(bestNode, action, newState);
                if (newNode->Reached(target))
                {
                    Plan* plan = new Plan(actor);
                    plan->AddFront(std::move(newNode->action), newState);
                    for (Node* previous = newNode->previous; previous != nullptr; previous = previous->previous)
                    {
                        if (previous->action)    // TODO: more gracefull detection of root node
                            plan->AddFront(std::move(previous->action), previous->state);
                    }
                    delete newNode;
                    return plan;
                }
                else
                {
                    open.insert(std::make_pair(newNode->Score(target, start.mp), std::unique_ptr<Node>(newNode)));
                }
            }
        }
        return nullptr;
    }
}   // ::Game


