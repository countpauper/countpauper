#pramga once
#include "Game/Stat.h"
#include <map>
#Include <string>

namespace Game
{

class Item
{
public:
        Item();
private:
        std::string name;
        std::map<Stat::Id, int> bonus;
        std::map<Stat::Id, int> enchantment;
};

}
