#pragma once 
#include "intrusive_list.hpp"
#include <string_view>
#include <expected>

namespace eul 
{
class timer_itf;


class profile : public intrusive_list::node
{
public:
    profile(timer_itf& timer);
    
    class  point : intrusive_list::node 
    {
    public:
        point(profile& profile, std::string_view name);
    private:
        std::string_view name; 
        float start;
        unsigned count;
    };

    point start(std::string_view name);
    point end(std::string_view name);

    inline std::expected<float, errno_t> total(std::string_view) const { return 0.0f; }
    inline unsigned count(std::string_view ) const { return 0; }

private:
    timer_itf& timer;
    intrusive_list points;
};



};