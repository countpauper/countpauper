#pragma once
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>
#include <span>

namespace Engine::Astar
{

template<typename T>
std::vector<T> ConstructPath(T current, const std::unordered_map<T, T>& previous)
{
    std::vector<T> result;
    while(true)
    {
        result.push_back(current);
        auto it = previous.find(current);
        if (it==previous.end())
            break;
        current = it->second;
    }
    return result;
}

template<typename T, typename Score>
bool UpdateOpen(std::multimap<Score, std::pair<T,Score>>& open, T found, Score score)
{
    for(auto it = open.begin(); it != open.end(); ++it)
    {
        if (it->second.first != found)
            continue;
        if (score < it->first)
        {
            open.erase(it);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

template<typename T, typename Score=double>
std::vector<T> Plan(T from, std::span<T> destinations, std::function<Score(T,T)> score, std::function<std::vector<T>(T)> neighbours)
{
    // a sorted map where the key is  cumulative Score to get to T + cost(T, to)
    // and the value is T and the score to get from T
    // the search direction is reversed to allow multiple targets
    std::multimap<Score, std::pair<T,Score>> open;
    for(auto to : destinations)
    {
        open.insert(std::make_pair(score(to, from), std::make_pair(to, 0)));
    }
    // path trace back for reconstruction at the end
    std::unordered_map<T, T> previous;
    // closed list to prevent infinite loops
    std::unordered_set<T> closed;
    while(!open.empty())
    {
        auto head = *open.begin();
        auto current = head.second.first;
        if (current == from)
        {
            return ConstructPath(from, previous);
        }
        open.erase(open.begin());
        closed.insert(current);
        for(auto neighbour : neighbours(current))
        {
            if (closed.count(neighbour))
                continue;
            Score neighbourCost = head.second.second +
                score(current, neighbour);
            Score neighbourScore = neighbourCost + score(neighbour, from);
            if (UpdateOpen(open, neighbour, neighbourScore))
            {
                open.insert(std::make_pair(neighbourScore, std::make_pair(neighbour, neighbourCost)));
                previous[neighbour] = current;
            }
        }
    }
    return std::vector<T>(); // failuire
}


template<typename T, typename Score=double>
std::vector<T> Plan(T from, T to, std::function<Score(T,T)> score, std::function<std::vector<T>(T)> neighbours)
{
    return Plan(from, std::span(&to, 1), score, neighbours);
}
}
