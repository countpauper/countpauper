#include "ContainerImpl.h"
#include "Logic/Expression.h"
#include "Logic/Internal/container_iterator.h"

namespace Angel::Logic
{

unsigned AddToContainer(Expression&& item, std::function<unsigned(Expression&&)> addOne)
{
    if (Association* association = item.GetIf<Association>())
    {
        assert(!association->Right().Is<All>()); // not implemented; 
        if (association->Right().Simplify() == False)
            return 0;
        if (association->Right().Simplify() == True)
            return AddToContainer(std::move(association->Left()), addOne);
        if (All* all = association->Left().GetIf<All>())
        {
            if (all->GetVariable())
                return addOne(*all);

            unsigned total = 0;
            for(auto subItem: *all)
            {
                total += AddToContainer(Association(std::move(subItem), std::move(association->Right())).Simplify(), addOne);
            }
            return total;            
        }     
    }
    if (const All* all = item.GetIf<All>())
    {
        if (all->GetVariable())
            return addOne(*all);

        unsigned total = 0;
        for(auto subItem: *all)
        {
            total += AddToContainer(std::move(subItem), addOne);
        }
        return total;
    }
    return addOne(std::move(item));
}


}