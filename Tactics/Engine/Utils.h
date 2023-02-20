#pragma once

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <string_view>
#include <chrono>

namespace Engine
{
    std::vector<std::wstring> Split(const std::wstring_view str, wchar_t delimiter);
    std::vector<std::string> Split(const std::string_view str, char delimiter);
    std::set<std::wstring> SplitSet(const std::wstring_view str, wchar_t delimiter);
    std::set<std::string> SplitSet(const std::string_view str, char delimiter);
	extern const std::wstring_view whitespace;
    std::wstring_view Strip(const std::wstring_view, std::wstring_view trash = whitespace);
    std::wstring UpperCase(const std::wstring_view str);
    std::string FormatDuration(double seconds);
    std::string FormatDuration(std::chrono::milliseconds);
    template<class T> 
    std::wstring ToWString(const T& object)
    {
        std::wstringstream stream;
        stream << object;
        return stream.str();
    }
    template<class T>
    std::string ToString(const T& object)
    {
        std::stringstream stream;
        stream << object;
        return stream.str();
    }

    struct Skip
    {
        explicit Skip(const std::wstring_view& skip) :
            characters(skip)
        {
        }
        std::wstring_view characters;
    };
    std::wistream& operator>>(std::wistream& s, Skip& c);

    struct ReadChar
    {
    public:
        explicit ReadChar(wchar_t c) :
            character(c)
        {
        }
        wchar_t character;
    };
    std::wistream& operator>>(std::wistream& s, ReadChar& c);

    struct ReadUntil
    {
        ReadUntil(wchar_t until, std::wstring& destination) :
            until(until),
            buffer(destination)
        {
        }

        wchar_t until;
        std::wstring& buffer;
    };
    std::wistream& operator>>(std::wistream& s, ReadUntil& c);
}