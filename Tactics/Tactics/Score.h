#pragma once
#include <string>
#include <vector>
namespace Game
{
    class Bonus
    {
    public:
        Bonus();
        Bonus(const std::wstring& description, int value);
        Bonus(const Bonus& other);
        operator bool() const { return value != 0;  }
        int value;
        std::wstring description;
    };

    class Score
    {
    public:
        Score();
        Score(unsigned value);
        Score(const Score& other);
        unsigned Evaluate() const;
        std::wstring Description() const;
        unsigned operator()() const { return Evaluate(); }
        Score& operator+=(const Bonus& other);
        Score& operator+=(const Score& other);
        Score& operator-=(const Score& other);
    private:
        friend std::wistream& operator>>(std::wistream& s, Score& score);
        unsigned base;
        std::vector<Bonus> boni;
    };
    Score operator+(const Score& a, const Score& b);

    std::wistream& operator>>(std::wistream& s, Score& score);


};
