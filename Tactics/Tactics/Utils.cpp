#include "stdafx.h"
#include "Utils.h"
#include <sstream>

namespace Engine
{

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

}