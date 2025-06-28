#include "Interpreter/Symbol.h"
#include <string>

namespace Interpreter 
{

hash_t SymbolHash(const std::string_view name)
{
    std::hash<std::string_view> hasher;
    return typeid(Interpreter::Symbol).hash_code() ^ hasher(name);
}

#ifdef NDEBUG
    Symbol::Symbol() :
        label(SymbolHash(""))
    {
    }

    Symbol::Symbol(const char* name) : 
        label(SymbolHash(name))
    {
    }

    Symbol::Symbol(const std::string_view name) :
        label(SymbolHash(name))
    {
    }
        
    hash_t Symbol::Hash() const
    {
        return label;
    }

    bool Symbol::operator<(const Symbol& other) const
    {
        return label < other.label;
    }

    Symbol::operator bool() const
    {
        return label != SymbolHash("");
    }

    std::size_t Symbol::Match(SourceSpan) const 
    { 
        return 0; 
    }
    bool Symbol::operator==(const Symbol& other) const
    {
        return label == other.label;
    }

    Symbol::operator std::string() const
    {
        return std::format("<{}>", label);
    }
#else 
    #warning Still in debug 
    Symbol::Symbol() = default;

    Symbol::Symbol(const char* name) : 
        name(name)
    {
    }

    Symbol::Symbol(const std::string_view name) :
        name(name)
    {
    }
        
    hash_t Symbol::Hash() const
    {
        return SymbolHash(name);
    }

    bool Symbol::operator<(const Symbol& other) const
    {
        return Hash() < other.Hash();
    }

    Symbol::operator bool() const
    {
        return !name.empty();
    }

    std::size_t Symbol::Match(SourceSpan) const 
    { 
        return 0; 
    }
    bool Symbol::operator==(const Symbol& other) const
    {
        return name == other.name;
    }

    Symbol::operator std::string() const
    {
        return std::format("<{}>", name);
    }
#endif

const Symbol Symbol::epsilon("ε");

std::ostream& operator<<(std::ostream& os, const Symbol& s)
{
    os << std::string(s).c_str();
    return os;
}

}

namespace std
{

size_t hash<Interpreter::Symbol>::operator()(const Interpreter::Symbol& s) noexcept 
{

    return s.Hash();
}

}