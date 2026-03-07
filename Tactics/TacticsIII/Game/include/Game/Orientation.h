#include "Geometry/Orientation.h"

namespace Game
{
    using Orientation = Engine::Orientation;
    using Orientations = Engine::Orientations;

	static const Orientation east = Orientation::right;
    static const Orientation west = Orientation::left;
    static const Orientation north = Orientation::front;
	static const Orientation south = Orientation::back;
    static const Orientation up = Orientation::up;
    static const Orientation down = Orientation::down;
}
