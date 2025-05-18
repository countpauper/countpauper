#pragma once 
#include <string_view>
#include <cstdint> 
#include <array>

namespace Angel::Logic
{

class Operator 
{
public:
    bool operator==(const Operator& rhs) const;
    explicit operator bool() const;
    operator std::string() const;
    const std::string_view Description() const;
    unsigned Precedence() const;
protected:
    constexpr explicit Operator(const wchar_t tag, uint8_t operands) :
        op{.sw{tag, operands, 0}}
    {
    }

    Operator(const std::string_view tags, uint8_t operands);
private:
    // low 2 bytes = wchar_t opreator, byte 3 = 0 termination, 's just the signle character tag's unicode. 
    union Code
    {
        uint32_t id;
        struct __attribute__ ((packed)) 
        {
            wchar_t unicode; // 1 unicode char in the Basic Multilingual Plane.
            uint8_t operands;
            uint8_t reserved;
        } sw;
    };
    Code op;
public: 
    struct Definition
    {
        Code code;
        std::int32_t precedence;
        std::string_view altTag;
        std::string_view description;
    };

};

class UnaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 1;

    UnaryOperator();
    constexpr explicit UnaryOperator(const wchar_t tag) : Operator(tag, operands)  {}
    UnaryOperator(const std::string_view tag);
private:
};

class BinaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 2;
    BinaryOperator();
    constexpr explicit BinaryOperator(const wchar_t tag) : Operator(tag, operands) {}
    BinaryOperator(const std::string_view tag);
};


std::ostream& operator<<(std::ostream& os, const Operator& list);

template < typename T >
concept IsOperation = std::derived_from<decltype(T::ope), Operator>;
}