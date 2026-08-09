#pragma once

#include "Utility/Range.h"
#include "Game/Position.h"
#include "Game/Cell.h"

#include <vector>
#include <span>
#include <initializer_list>
#include <iterator>

namespace Game
{

class Material;

class Stack 
{
public:
    Stack() = default;
    Stack(const Stack& other);
    Stack& operator=(const Stack& other);
    Stack(const Material& material, Cell::Height height=1.0, Cell::Temperature temp=300.0);
    Stack(std::initializer_list<Cell> cells);

    void emplace_back(const Material& material, Cell::Height height, Cell::Temperature temp);
    Cell pop_front();

    using Cells = std::vector<Cell>;
    using const_iterator =Cells::const_iterator; 
    using const_reverse_iterator = Cells::const_reverse_iterator;
    using iterator = Cells::iterator;
    using value_type = Cell; 
    
    inline std::size_t size() const { return cells.size(); }
    inline const_iterator begin() const { return cells.begin(); }
    inline const_iterator end() const { return cells.end(); }
    inline iterator begin() { return cells.begin(); }
    inline iterator end() { return cells.end(); }    
    inline const_reverse_iterator rbegin() const { return cells.rbegin(); }
    inline const_reverse_iterator rend() const { return cells.rend(); } 
    // Return the iterator of the cell at 'height'  and the amount that height is above the bottom of that cell
    std::pair<const_iterator, Cell::Height> Find(ZType height) const;
    std::pair<iterator,Cell::Height> Find(ZType height);
    inline const Cell& operator[](unsigned idx) const { return cells.at(idx); }
    inline Cell& operator[](unsigned idx) { return cells.at(idx); }

    
    Stack& operator+=(const Stack&);
    Stack& operator&=(Engine::Range<ZType> heigh);
    Stack& operator*=(float scale);

    Engine::Range<ZType> FindBiggestOpening() const;
    Engine::Range<ZType> FindBiggestNonSolidOpening() const;
    Engine::Range<ZType> FindBiggestRange(Cell::Predicate predicate) const;

    auto Find(const_iterator from, Cell::Predicate to) const 
    {
        return std::ranges::subrange(from, std::find_if(from, cells.end(), to));
    }

    auto FindBackwards(const_iterator from, Cell::Predicate to) const 
    {
        
        if (from == cells.end())
            return std::ranges::subrange(cells.end(), cells.end());
        auto it = from;
        for(; to(*it); --it)
        {
            if (it==cells.begin())
                return std::ranges::subrange(it, from+1);                
        }
        return  std::ranges::subrange(it+1, from+1);
    }
    iterator Fill(Engine::Range<ZType> height, const Material& material, Cell::Temperature temperature);
private:
    Cells cells;
};

Stack operator+(const Stack& lhs, const Stack& rhs);
Stack operator&(const Stack& lhs, Engine::Range<ZType> rng);
Stack operator*(const Stack& lhs, float scale);




}
