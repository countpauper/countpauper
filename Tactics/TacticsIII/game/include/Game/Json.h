#pragma once
#include <nlohmann/json.hpp>
#include <optional>

namespace Game
{
using json = nlohmann::json;

template<typename T>
std::optional<T> try_get(const json& j, std::string_view key)
{
        auto it = j.find(key);
        if (it == j.end())
        {
                return std::optional<T>();
        }
         else
        {
                return it.value().template get<T>();
        }
}

template<>
std::optional<json> try_get<json>(const json& j, std::string_view key);

template<typename T>
T get_value_or(const json& j, std::string_view key, T&& alternative=T())
{
        if (auto value = try_get<T>(j, key))
        {
                return *value;
        }
        else
        {
                return std::move(alternative);
        }
}

}
