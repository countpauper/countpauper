#include "Engine/Utility/Utils.h"
#include <sstream>
#include <ctype.h>
#include <iomanip>
#include <algorithm>

namespace Engine
{

const std::string_view whitespace(" \n\r\t");


std::vector<std::string_view> Split(std::string_view str, char delimiter)
{
    std::vector<std::string_view> result;
    if (str.empty())
        return result;
    auto pos = 0;
    char delimiters[2]{delimiter, 0};

    while(true)
    {
        auto next = str.find_first_of(delimiters, pos);
        if (next == std::string_view::npos) {
            result.push_back(str.substr(pos));
            break;
        } else {
            result.push_back(str.substr(pos, next-pos));
            pos = next +1;
        }
    }
    return result;
}


std::set<std::string_view> SplitSet(std::string_view str, char delimiter)
{
    auto splitVector = Split(str, delimiter);
    return std::set<std::string_view>(splitVector.begin(), splitVector.end());
}

std::string UpperCase(std::string_view str)
{
    std::string result(str.length(), '\x0');
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return result;
}

std::string LowerCase(std::string_view str)
{
    std::string result(str.length(), '\x0');
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);
    return result;
}

std::string FormatDuration(double seconds)
{
    if (seconds >=0)
        return FormatDuration(std::chrono::milliseconds(static_cast<uint64_t>(seconds*1000.0)));
    else
        return "-" + FormatDuration(std::chrono::milliseconds(static_cast<uint64_t>(-seconds*1000.0)));
}

std::string FormatDuration(std::chrono::milliseconds duration)
{
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration - hours - minutes - seconds);

    std::stringstream ss;
    ss << std::setfill('0') << hours.count() << ":" <<
        std::setw(2)  << minutes.count() << ":" <<
        std::setw(2)  << seconds.count() << "." <<
        std::setw(3) << millis.count();
    return ss.str();
}

std::string_view Strip(std::string_view str, std::string_view trash)
{
    auto start = str.find_first_not_of(trash);
    if (start == std::string::npos)
        return std::string();
    auto end = str.find_last_not_of(trash);
    if (end == std::string::npos)
        return str.substr(start);
    else
        return str.substr(start, 1+end - start);
}

std::istream& operator>>(std::istream& s, Skip skip)
{
    while (s.good())
    {
        wchar_t nextChar = s.peek();
        if (skip.characters.find_first_of(nextChar) == std::string::npos)
            return s;
        s.get();
    }
    return s;
}


std::istream& operator>>(std::istream& s, ReadChar c)
{
    if (s.peek() != c.character)
        throw std::runtime_error((std::string("Failed to read '") + (char)c.character + "'").c_str());
    s.get();
    return s;
}

std::istream& operator>>(std::istream& s, ReadUntil read)
{
    std::string result;
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
