#include "stdafx.h"
#include "Utils.h"
#include <sstream>
#include <ctype.h>


namespace Engine
{

const std::wstring_view whitespace(L" \n\r\t");

    
std::vector<std::wstring> Split(const std::wstring_view str, wchar_t delimiter)
{
    std::vector<std::wstring> result;
    if (str.empty())
        return result;
    std::wstringstream ss(str.data());
    while (!ss.eof())
    {
        static const std::streamsize bufferSize(4096);
        static wchar_t itemBuffer[bufferSize];
        ss.getline(itemBuffer, bufferSize, delimiter);
        if (ss.gcount() == bufferSize)
            throw std::runtime_error("Element in split string too long ");
        result.emplace_back(itemBuffer);
    }
    return result;
}

std::vector<std::string> Split(const std::string_view str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str.data());
    while (!ss.eof())
    {
        static const std::streamsize bufferSize(4096);
        static char itemBuffer[bufferSize];
        ss.getline(itemBuffer, bufferSize, delimiter);
        if (ss.gcount() == bufferSize)
            throw std::runtime_error("Element in split string too long ");
        result.emplace_back(itemBuffer);
    }
    return result;
}

std::set<std::wstring> SplitSet(const std::wstring_view str, wchar_t delimiter)
{
    auto splitVector = Split(str, delimiter);
    return std::set<std::wstring>(splitVector.begin(), splitVector.end());
}

std::set<std::string> SplitSet(const std::string_view str, char delimiter)
{
    auto splitVector = Split(str, delimiter);
    return std::set<std::string>(splitVector.begin(), splitVector.end());
}

std::wstring UpperCase(const std::wstring_view str)
{
    std::wstring result(str.length(), '\x0');
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}


std::wstring_view Strip(const std::wstring_view str, const std::wstring_view trash)
{
    auto start = str.find_first_not_of(trash);
    if (start == std::wstring::npos)
        return std::wstring();
    auto end = str.find_last_not_of(trash);
    if (end == std::wstring::npos)
        return str.substr(start);
    else
        return str.substr(start, 1+end - start);
}

std::wistream& operator>>(std::wistream& s, Skip& skip)
{
    while (s.good())
    {
        wchar_t nextChar = s.peek();
        if (skip.characters.find_first_of(nextChar) == std::wstring::npos)
            return s;
        s.get();
    }
    return s;
}


std::wistream& operator>>(std::wistream& s, ReadChar& c)
{
    if (s.peek() != c.character)
        throw std::runtime_error((std::string("Failed to read '") + (char)c.character + "'").c_str());
    s.get();
    return s;
}

std::wistream& operator>>(std::wistream& s, ReadUntil& read)
{
    std::wstring result;
    while (s.good())
    {
        if (read.until == s.peek())
        {
            s.get();
            break;
       }
        read.buffer += s.get();
    }
    return s;

}




}