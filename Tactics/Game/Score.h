#pragma once
#include <string>
#include <vector>

namespace Game
{
    class Bonus
    {
    public:
        Bonus();
        explicit Bonus(int value);
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
        explicit Score(const Bonus& bonus);
        Score(const Score& other);
        Score(const std::wstring& description, int value);
        unsigned Value() const;
        std::wstring Description() const;
        unsigned operator()() const { return Value(); }
        Score& operator+=(const Bonus& bonus);
        Score& operator+=(const Score& other);
        Score& operator-=(const Score& other);
        bool operator==(const Score& other) const;
        bool operator==(unsigned value) const;
    private:
        friend std::wistream& operator>>(std::wistream& s, Score& score);
        friend std::ostream& operator<<(std::ostream& s, const Score& score);
        std::vector<Bonus> boni;
    };
    Score operator+(const Score& a, const Bonus& b);
    Score operator+(const Score& a, const Score& b);

    std::wistream& operator>>(std::wistream& s, Score& score);
    std::ostream& operator<(std::ostream& s, const Score& score);


};
