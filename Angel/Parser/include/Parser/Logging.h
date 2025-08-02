#pragma once 

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

namespace Logging 
{
    constexpr std::ostream* OFF = nullptr;
    constexpr std::ostream* ERROR = &std::clog;
    constexpr std::ostream* WARN = &std::clog;
#ifdef RELEASE
    constexpr std::ostream* INFO = OFF;
    constexpr std::ostream* DEBUG = OFF;
#else 
    constexpr std::ostream* INFO = &std::cout;
//    constexpr std::ostream* DEBUG = &std::cout;
    constexpr std::ostream* DEBUG = OFF;
#endif
    template<std::ostream* s, class... Args>
    inline typename std::enable_if_t<s!=nullptr>
    Log( std::format_string<Args...> fmt, Args&&... args )
    {
        (*s) << std::format("[{:8}] ",std::clock()/1000.0);
        (*s) << std::format(fmt, std::forward<Args>(args)...) << std::endl;
    }

    template<std::ostream* s, class... Args>
    inline typename std::enable_if_t<s==nullptr>
    Log( std::format_string<Args...>, Args&&...)
    {
    }


    class Tabber
    {
    public:
        Tabber& operator++()
        {
            tabs ++;
            return *this;
        }
        Tabber& operator--()
        {
            tabs--;
            return *this;
        }
        Tabber& operator=(int tab)
        {
            tabs = tab;
            return *this;
        }
        operator bool() const 
        {
            return tabs!=0;
        }
        operator::std::string() 
        {
            return std::string(tabs, ' ');
        }
    private:
        int tabs = 0;
    };

}