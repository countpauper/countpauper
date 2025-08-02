#pragma once 
#include <string_view>
#include <cstdint> 
#include <array>
#include <vector>

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
    bool NeedsBracesAround(const Expression& expresison, bool first) const;
    uint32_t Id() const { return op.id; }
    unsigned Operands() const { return op.sw.operands; }
    bool IsPostfix() const { return op.sw.postfix; }
    bool IsPrefix() const { return !IsPostfix(); }
    bool IsComparator() const { return op.sw.comparison; }
    template<typename... T> 
    static Operator Find(std::string_view tag);
    static std::vector<Operator> All();
protected:
    constexpr Operator(const wchar_t tag, uint8_t operands, bool postfix=false, bool comparator=false) :
        op{.sw{tag, operands, postfix, comparator, 0, 0}}
    {
    }
    Operator(const std::string_view tags, uint8_t operands);
    constexpr explicit Operator(uint32_t code) : op{.id{code}} {}
    static uint32_t FindId(const std::string_view tag, unsigned operands);
private:
    union Code
    {
        uint32_t id;
        struct __attribute__ ((packed)) 
        {
            wchar_t unicode; // 1 unicode char in the Basic Multilingual Plane.
            uint8_t operands:2; // noperator 0, unary, binary for pairs, 3 for multiary  
            bool postfix:1;
            bool comparison:1;
            uint8_t reserved:4;
            uint8_t reserved2;
        } sw;
    };
public: 
    Code op;
};


template < typename T >
concept is_operation = std::derived_from<decltype(T::ope), Operator>;

std::ostream& operator<<(std::ostream& os, const Operator& op);


}