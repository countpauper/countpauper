#include "stdafx.h"
#include "Utils.h"
#include <sstream>

namespace Engine
{

const wchar_t* whitespace=L" \n\r\t";

    
std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter)
{
    std::vector<std::wstring> result;
    std::wstringstream ss(str);
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

std::vector<std::string> Split(const std::string& str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
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
std::set<std::string> SplitSet(const std::string& str, char delimiter)
{
    auto splitVector = Split(str, delimiter);
    return std::set<std::string>(splitVector.begin(), splitVector.end());
}

}