#include "Interpreter/Utils.h"
#include <stdexcept>
#include <format>

namespace Interpreter
{

std::string Unclose(const std::string_view in, char left, char right)
{
    if (in.size()<2)
        throw std::invalid_argument("Can't unclose a string smaller than the left and right enclosure");
    if (in.front()!=left)
        throw std::invalid_argument(std::format("`{}` is not enclosed with `{}` on the left", in, left));
    if (in.back()!=right)
        throw std::invalid_argument(std::format("`{}` is not enclosed with `{}` on the right", in, right));
    return std::string(in.data()+1, in.size()-2);
}

std::string Unclose(const std::string_view in, char c)
{
    return Unclose(in, c, c);
}

}