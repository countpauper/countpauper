#include "Interpreter/Terms.h"
#include "Interpreter/Utils.h"
#include "Interpreter/Source.h"
#include <regex>
#include <cassert>
#ifdef WIN32
// on windows wide charactes are 16 bit. To still support code pages >=0x10000 use the windows 
// conversion functions, which support surrogate pairs for them
#include <windows.h>
std::wstring utf8_to_wstring(std::string_view utf8) 
{
    int len = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, utf8.data(), utf8.size(), nullptr, 0);
    if (len == 0) {
        throw std::invalid_argument("Wide character conversion error");
    }
    std::wstring wide(len, L'\0');
    auto success = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size(), wide.data(), len);
    if (!success)
    {
        DWORD err = ::GetLastError();
        throw std::system_error(static_cast<int>(err),std::system_category(), "Failed to convert utf8 to wide char");
    }
    return wide;
}
std::string wstring_to_utf8(std::wstring_view wstr) 
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), NULL,0, NULL, NULL);
    std::string utf8(len, '\0');
    auto success = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.data(), wstr.size(), utf8.data(), len, NULL, NULL);
    if (!success)
    {
        DWORD err = ::GetLastError();        
        throw std::system_error(static_cast<int>(err),std::system_category(), "Failed to convert wide char to utf8");
    }
    return utf8;
}

#else 
std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconv;
std::wstring utf8_to_wstring(std::string_view str) 
{
    return strconv.from_bytes(str.begin(), str.end());
}
std::string wstring_to_utf8(std::wstring_view str) 
{
    return strconv.to_bytes(str.begin(), str.end());
}


#endif

namespace Interpreter
{

Literal::Literal(const std::string_view match):
    match(match)
{
    if (match.empty())
        throw std::invalid_argument("Literal token can not be empty. Use Epsilon.");
}

std::size_t Literal::Match(SourceSpan src) const
{
    if (src.size()>=match.size() && 
        std::equal(match.begin(), match.end(), src.begin()))
        return match.size();
    else
        return 0;
}

Literal::operator std::string() const
{
    return std::format("\"{}\"", match.c_str());
}

bool Literal::operator==(const Literal& other) const
{
    return other.match == match;
}


Regex::Regex(const std::string_view match):
    wmatch(utf8_to_wstring(match)),
    expression(ConstructExpression())
{
    if (wmatch.empty())
        throw std::invalid_argument("Regex token can not be empty. Use Epsilon.");
}

std::unique_ptr<std::wregex> Regex::ConstructExpression() const
{
    return std::make_unique<std::wregex>(wmatch.data(), wmatch.size(), 
        std::regex_constants::ECMAScript | 
        std::regex_constants::optimize | 
        std::regex_constants::nosubs);
}

Regex::Regex(const Regex& other) :
    wmatch(other.wmatch),
    expression(ConstructExpression())    
{
}

Regex::Regex(Regex&& other) :
    wmatch(std::move(other.wmatch)),
    expression(std::move(other.expression))
{
}

Regex& Regex::operator=(const Regex& other)
{
    wmatch = other.wmatch;
    expression.reset();
    expression = ConstructExpression();
    return *this;
}

Regex& Regex::operator=(Regex&& other)
{
    expression.reset();
    wmatch.clear();
    std::swap(wmatch, other.wmatch);
    std::swap(expression, other.expression);
    return *this;
}

std::size_t length_of_next_utf8_codepoint(std::string_view utf8_string)
{
    if (utf8_string.empty())
        return 0;
    char first = utf8_string.front();
    if ((first&0x80) == 0) 
        return 1;
    else if ((first&0xE0) == 0xC0)
        return 2;
    else if ((first&0xF0) == 0xE0)
        return 3;
    else
        return 4;
}

std::size_t wchars_to_bytes(std::string_view utf8_string, std::size_t wchars)
{ 
    size_t byte_offset = 0;
    for (size_t cp = 0; cp < wchars; ++cp) 
    {
        size_t char_bytes = length_of_next_utf8_codepoint(utf8_string.substr(byte_offset, -1)); 
        byte_offset += char_bytes;
    }
    return byte_offset;
}

std::size_t Regex::Match(SourceSpan src) const
{
// TODO: lots of source copy and converting. Instead create a "view" iterator that 
// 1) Moved the byte pointer by the right code point every operator++
// 2) Returns a wchar_t with operator*
// 3) is bidirectional, by caching all code point sizes increased so far 
// 4) can return the byte offset as well, based on summing that cache
//    std::match_results<SourceSpan::const_iterator> result;
    std::wsmatch result;
    std::string utf8Source = src.extract(); 
    std::wstring theSource = utf8_to_wstring(utf8Source);
    if (std::regex_search(theSource, result, *expression, 
            std::regex_constants::match_continuous | std::regex_constants::match_not_null))
        return wchars_to_bytes(utf8Source, result.length());
    else 
        return 0;
}

Regex::operator std::string() const
{
    return std::format("'{}'", wstring_to_utf8(wmatch));
}

bool Regex::operator==(const Regex& other) const
{
    return other.wmatch == wmatch;
}

std::size_t Match(const Term& token, SourceSpan src)
{
    auto matcher = [&src](const auto& obj) { return obj.Match(src); };
    return std::visit(matcher, token);
}

Epsilon::Epsilon(const Symbol& symbol) :
    symbol(symbol)
{
}

const Symbol& Epsilon::GetSymbol() const
{
    return symbol;
}

std::size_t Epsilon::Match(SourceSpan) const
{
    return 0;
}

Epsilon::operator std::string() const
{
    if (symbol && symbol!=Symbol::epsilon)
        return std::format("ε{}", std::string(symbol));
    else
        return "ε";
}

bool Epsilon::operator==(const Epsilon& other) const
{
    return other.symbol == symbol;
}


}

namespace std
{


size_t hash<Interpreter::Literal>::operator()(const Interpreter::Literal& l) noexcept 
{
    std::hash<std::string> hasher;
    return hasher(l.match);
}

size_t hash<Interpreter::Regex>::operator()(const Interpreter::Regex& r) noexcept 
{
    std::hash<decltype(r.wmatch)> hasher;
    return hasher(r.wmatch);
}

size_t hash<Interpreter::Epsilon>::operator()(const Interpreter::Epsilon& e) noexcept 
{
    std::hash<std::string> hasher;
    return e.symbol.Hash();
}

string to_string(const Interpreter::Term& term)
{
    return visit(
        [](const auto& obj) { return string(obj); },
        term);
}

}