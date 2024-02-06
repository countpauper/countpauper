#pragma once
#include <string>
#include <vector>

namespace Game
{


class StatDescriptor
{
public:

        StatDescriptor();
        void Contribute(std::string_view source, int value, bool skip0=true);
        int Total() const;
        std::string Description() const;
private:
        std::vector<std::pair<std::string_view, int>> contributions;
};

}
