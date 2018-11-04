#pragma once
#include <string>
#include <vector>
namespace Game
{
    class Bonus
    {
    public:
        Bonus();
        Bonus(int value);
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
        Score& operator+=(const Bonus& bonus);
        Score& operator+=(const Score& other);
    private:
        friend std::wistream& operator>>(std::wistream& s, Score& score);
        std::vector<Bonus> boni;
    };
    Score operator+(const Score& a, const Bonus& b);
    Score operator+(const Score& a, const Score& b);

    std::wistream& operator>>(std::wistream& s, Score& score);


};
