#pragma once

#include <vector>

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
            Item(const Actor& actor);
            Item(const Item&) = delete;
            Item(Item&& other);
            void Render() const;
            void Highlight(bool on);
            const Actor& actor;
        private:
            bool highlighted;
        };

        TurnList(Game& game, unsigned width);
        void Render() const;
        unsigned Width() const;
        void Key(unsigned short code);
    private:
        void Update();
        Game& game;
        std::vector<Item> items;
        unsigned width;
        boost::signals2::scoped_connection actorConnection;
    };
}