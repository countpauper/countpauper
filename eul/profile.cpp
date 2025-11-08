#include "profile.hpp"
#include "timer_itf.hpp"

namespace eul 
{

profile::profile(timer_itf& timer) :
    timer(timer)
{
}


profile::point::point(profile& prof, std::string_view name) 
    : intrusive_list::node(prof)
    , name(name)
    , count(0)
    , start(0)
{
}

}