#include "Game/Orientation.h"

namespace Game
{

const Orientation east = Orientation::right;
const Orientation west = Orientation::left;
const Orientation north = Orientation::front;
const Orientation south = Orientation::back;
const Orientation up = Orientation::up;
const Orientation down = Orientation::down;


bool IsProne(const Orientation ori)
{
    return ori.IsVertical();
}

}