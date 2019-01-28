#include "stdafx.h"
#include "Slot.h"

namespace Game
{

    std::wistream& operator>>(std::wistream& s, Slot& slot)
    {
        std::wstring slotName;
        s >> slotName;
        slot = slotMap.at(slotName);
        return s;
    }

}