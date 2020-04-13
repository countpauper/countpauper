#pragma once

#include <vector>
#include <connection.h>

namespace Game
{
    class Game;
    class Actor;

    class TurnList
    {
    public:
        class Item
        {
        public:
            Item(const Actor& actor, unsigned hotKey);
            Item(const Item&) = delete;
            Item(Item&& other);
            void Render() const;
            void Highlight(bool on);
            unsigned Height() const;
            unsigned HotKey() const;
            const Actor& actor;
        private:
            void RenderBackground() const;
            void RenderActor() const;
            void RenderPlan() const;
            void RenderState() const;
            bool highlighted;
            unsigned hotKey;
        };

        TurnList(Game& game, unsigned width);
        void Render() const;
        unsigned Width() const;
        void Key(unsigned code);
    private:
        void Update();
        Game& game;
        std::vector<Item> items;
        unsigned width;
        is::signals::scoped_connection selectedConnection;
        is::signals::scoped_connection activatedConnection;
    };
}