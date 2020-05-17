#pragma once

#include <string>
#include <vector>
#include <set>

namespace Engine
{
    std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter);
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::set<std::wstring> SplitSet(const std::wstring& str, wchar_t delimiter);
    std::set<std::string> SplitSet(const std::string& str, char delimiter);
	extern const std::wstring_view whitespace;
    std::wstring Strip(const std::wstring& str, const std::wstring_view& trash = whitespace);
    std::string UpperCase(const std::string& str);


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