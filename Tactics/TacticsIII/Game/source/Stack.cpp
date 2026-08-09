#include "Game/Stack.h"
#include "Utility/Assert.h"

namespace Game
{

Stack::Stack(const Stack& other) 
    : cells(other.cells)
{
}

Stack::Stack(const Material& mat, Cell::Height height, Cell::Temperature temp) :
    Stack{{{mat, height, temp}}}
{
}

Stack::Stack(std::initializer_list<Cell> cells) : 
    cells(cells)
{
}


void Stack::emplace_back(const Material& material, Cell::Height height, Cell::Temperature temp)
{
    if (!cells.empty())
    {
        if (cells.back().TryMerge(Cell{material, height, temp}))
            return;
    }
    cells.emplace_back(material, height, temp);
}


std::pair<Stack::const_iterator, Cell::Height> Stack::Find(ZType height) const
{
    Cell::Height progress{0.0};
    for(auto it = begin(); it!=end(); ++it)
    {
        if (progress + it->height > height)
            return std::make_pair(it, height - progress);
        progress += it->height;
    }
    return std::make_pair(end(), Cell::Height());
}

std::pair<Stack::iterator, Cell::Height> Stack::Find(ZType height)
{   // avoid duplicating Find algorithm by "const casting" the iterator 
    auto[cit, amount] = const_cast<const Stack*>(this)->Find(height);
    unsigned idx = cit - const_cast<const Stack*>(this)->begin();
    return std::make_pair(cells.begin() + idx, amount);
}

Stack& Stack::operator=(const Stack& rhs)
{
    cells = rhs.cells;
    return *this;
}
 
Stack& Stack::operator+=(const Stack& rhs)
{
    if (cells.empty()) 
    {
        cells = rhs.cells;
        return *this;
    }
    auto start = rhs.cells.begin();
    while (start!= rhs.cells.end() && 
            cells.back().TryMerge(*start))
    {
        ++start;
    }   
    cells.insert(cells.end(), start, rhs.cells.end());
    return *this;
}

Stack operator+(const Stack& lhs, const Stack& rhs)
{
    Stack result(lhs);
    return result += rhs;
}

Stack& Stack::operator&=(Engine::Range<ZType> height)
{
    if (height.begin > height.end) {
        cells.clear();
        return *this;
    }
    auto cutIt = cells.begin();
    if (height.begin < ZType(0))
    {
        cutIt = cells.insert(cutIt, {Material::vacuum, -height.begin, 0.0});
        ++cutIt;
        height.begin = ZType(0);
    }
    Cell::Height progress { static_cast<Cell::Height::IntegerType>(0) };
    auto cutBegin = cutIt;
    while(cutIt!=cells.end())
    {
        if (progress + cutIt->height >= height.begin) 
        {
            auto cutProgress = height.begin - progress;
            progress += cutProgress;
            cutIt->height -= cutProgress;
            break;
        }
        progress += cutIt->height;
        cutIt = cells.erase(cutIt);
    }

    auto cutEnd = cutIt;
    while(cutIt!=cells.end()) 
    {
        if (progress + cutIt->height >= height.end)
        {
            auto cutProgress = height.end - progress;
            cutIt->height = cutProgress;
            progress += cutProgress;
            cutEnd = ++cutIt;
            cutIt = cells.erase(cutEnd, cells.end());
            break;
        }
        progress += cutIt->height;
        ++cutIt;
    }
    if (progress < height.end)
    {
        (*this)+=Stack({Material::vacuum, height.end - progress, 0.0});
    }
    return *this;
}

Stack operator&(const Stack& lhs, Engine::Range<ZType> rng)
{
    Stack result(lhs);
    return result &= rng;
}


Stack& Stack::operator*=(float scale)
{
    for(auto& cell: cells)
    {
        cell.height *= scale;
    }
    return *this;
}

Stack operator*(const Stack& lhs, float scale)
{
    Stack result(lhs);
    return result *= scale;
}


Engine::Range<ZType> Stack::FindBiggestOpening() const
{
    return FindBiggestRange([](const Cell& l)
    {
        return l.IsCompressible();
    });
}

Engine::Range<ZType> Stack::FindBiggestNonSolidOpening() const
{
    return FindBiggestRange(std::mem_fn(&Cell::IsGas));
}

Engine::Range<ZType> Stack::FindBiggestRange(std::function<bool(const Cell&)> predicate) const
{
    auto result = Engine::Range<ZType>::empty();
    ZType progress = 0;
    Engine::Range<ZType> current = result;
    for(const auto& cell : cells)
    {
        if (predicate(cell))
        {
            current.begin = std::min(progress, current.begin);
        }
        else if (!predicate(cell))
        {
            current.end = progress; 
            if (current.Size() > result.Size())
                result = current;
            current = Engine::Range<ZType>::empty();
        }
        progress += cell.height;
    }
    current.end = progress; 

    if (current.Size() > result.Size())
        result = current;

    return result;

}

Stack::iterator Stack::Fill(Engine::Range<ZType> height, const Material& material, Cell::Temperature temperature)
{
    auto[it, offset] = Find(height.begin);
    Cell::Height displaced = 0.0;    // the volume removed to make room for the height
    Cell::Height level = SumHeight(std::ranges::subrange(begin(), it));
    if (it!=cells.end() && offset!=0.0)
    {
        displaced += it->height - offset;
        level += it->height;
        it->height = offset;
        if (displaced > height.Size())
        {   // split this overlapping cell 
            it = cells.emplace(it+1, it->material, displaced - height.Size(), it->temperature);
            return cells.emplace(it, material, height.Size(), temperature);
        }
        ++it;
        // TODO: later (as an option?) increase pressure by keeping the mass in a reduced volume 
    }
    auto replaceStart = it;
    while(it!= cells.end() && level + it->height <= height.end)
    {
        level+=it->height;
        displaced += it->height;
        ++it;
    }
    // TODO: what to do with the mass of the replaced? Compress it into the cell above in some balance?
    it = cells.erase(replaceStart, it);
    if (it!=cells.end())
    {
        it->height -= height.Size() - displaced;
        // TODO increase pressure 
    }
    return cells.emplace(it, material, height.Size(), temperature);

}

}
