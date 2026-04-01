#include "Geometry/Orientation.h"

namespace Game
{
    using Orientation = Engine::Orientation;
    using Orientations = Engine::Orientations;

	extern const Orientation east;
    extern const Orientation west;
    extern const Orientation north;
	extern const Orientation south;
    extern const Orientation up;
    extern const Orientation down;

    inline bool IsProne(const Orientation ori);
}
