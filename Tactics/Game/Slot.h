#pragma once
#include <map>

namespace Game
{
    enum class Slot : unsigned
    {
        Nothing = 0,
        Belly = 1,
        Chest = 2,
        Arms = 4,
        Legs = 8,
        Head = 16,
        Finger = 32,
        Neck = 64,

        Trunk = Belly + Chest,
        Shirt = Trunk + Arms,
        Body = Trunk + Arms + Legs,
        Full = Body + Head,
    };


    static const std::map<const std::wstring_view, Slot> slotMap({
        { L"Nothing", Slot::Nothing },
        { L"Belly", Slot::Belly },
        { L"Chest", Slot::Chest },
        { L"Arms", Slot::Arms },
        { L"Legs", Slot::Legs },
        { L"Head", Slot::Head },
        { L"Finger", Slot::Finger },
        { L"Neck", Slot::Neck },
        { L"Trunk", Slot::Trunk },
        { L"Shirt", Slot::Shirt },
        { L"Body", Slot::Body },
        { L"Full", Slot::Full },
    });
    std::wistream& operator>>(std::wistream& s, Slot& slot);
}    // ::Game
