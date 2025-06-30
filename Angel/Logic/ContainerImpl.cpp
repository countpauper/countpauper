#include "ContainerImpl.h"
#include "Logic/Expression.h"
#include "Logic/Internal/container_iterator.h"

namespace Angel::Logic
{

unsigned AddToContainer(const_container_iterator& at, Expression&& item, std::function<unsigned(const_container_iterator& at, Expression&&)> addOne)
{
    if (!bool(item))
        return 0;

    if (Association* association = item.GetIf<Association>())
    {
        assert(!association->Right().Is<All>()); // not implemented; 
        if (association->Right() == False)
            return 0;
        if (association->Right() == True)
            return AddToContainer(at, std::move(association->Left()), addOne);
        if (All* all = association->Left().GetIf<All>())
        {
            if (all->GetVariable())
                return addOne(at, *all);
            unsigned total = 0;
            for(auto subItem: *all)
            {
                total += AddToContainer(at, Association(std::move(subItem), std::move(association->Right())), addOne);
                ++at;
            }
            return total;            
        }     
    }
    if (const All* all = item.GetIf<All>())
    {
        if (all->GetVariable())
            return addOne(at, *all);

        unsigned total = 0;
        for(auto subItem: *all)
        {
            total += AddToContainer(at, std::move(subItem), addOne);
            ++at;
        }
        return total;
    }
    return addOne(at, std::move(item));
}


}