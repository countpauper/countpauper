#pragma once 
#include <string_view>
#include <cstdint> 
#include <array>

namespace Angel::Logic
{

class Expression;
class Operator 
{
public:
    constexpr Operator() : Operator(L'\000',0) {};
    constexpr bool operator==(const Operator& rhs) const { return op.id == rhs.op.id; }
    explicit operator bool() const;
    operator std::string() const;
    const std::string_view Description() const;
    unsigned Precedence() const;
    uint32_t Id() const { return op.id; }
    unsigned Operands() const { return op.sw.operands; }
    bool Postfix() const { return op.sw.postfix; }
protected:
    constexpr explicit Operator(const wchar_t tag, uint8_t operands, bool postfix=false) :
        op{.sw{tag, operands, postfix, 0, 0}}
    {
    }
    Operator(const std::string_view tags, uint8_t operands);
private:
    union Code
    {
        uint32_t id;
        struct __attribute__ ((packed)) 
        {
            wchar_t unicode; // 1 unicode char in the Basic Multilingual Plane.
            uint8_t operands:2; // noperator 0, unary, binary. TODO 3 for multiary as opposed to strictly binary . and : ? 
            bool postfix:1;
            uint8_t reserved:5;
            uint8_t reserved2;
        } sw;
    };
public: 
    Code op;
};


std::ostream& operator<<(std::ostream& os, const Operator& op);

template < typename T >
concept IsOperation = std::derived_from<decltype(T::ope), Operator>;
}