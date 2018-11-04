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
        Score(const Score& other);
        unsigned Value() const;
        std::wstring Description() const;
        unsigned operator()() const { return Value(); }
        Score& operator+=(const Bonus& other);
    private:
        friend std::wistream& operator>>(std::wistream& s, Score& score);
        unsigned value;
        std::wstring description;
    };
    Score operator+(const Score& a, const Bonus& b);

    std::wistream& operator>>(std::wistream& s, Score& score);


};
