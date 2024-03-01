#include "File/Json.h"

namespace Engine
{

template<>
std::optional<json> try_get<json>(const json& j, std::string_view key)
{
        auto it = j.find(key);
        if (it == j.end())
        {
                return std::optional<json>();
        }
         else
        {
                return it.value();
        }
}


}
