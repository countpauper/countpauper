#include "Game/Utilities.h"
#include "Game/Actor.h"
#include "Game/World.h"
#include "Game/HeightMap.h"
#include "Game/Slice.h"
#include "Geometry/Line.h"

namespace Game
{

Engine::Orientations Facing(Position from, Position to)
{
    Engine::Orientations result;
    Position V = to - from;
    return Engine::Orientations(Engine::Position{V.X(), V.Y(), static_cast<int>(std::round(V.Z()))});
}

void DownPositions(std::vector<std::pair<Engine::Position, double>>& path)
{
    for(auto it = path.begin(); it!=path.end();)
    {
        auto totalLength = it->second;
        auto nit = it;
        auto minZ = it->first.z;
        for(++nit; nit!=path.end();++nit)
        {
            if (it->first.x != nit->first.x || 
                it->first.y != nit->first.y)
                break;
            totalLength += nit->second;
            minZ = std::min(minZ, nit->first.z);
        }
        it->second = totalLength;
        it->first.z = minZ;
        it = path.erase(it+1, nit);
    }
}



double ComputeAttackSurface(const World& world, const Actor& from, const Actor& to)
{
    Engine::Range<float> aimHeight{0,1};
    // TODO add attack height and target height based on state and size etc, weapon reach 
    // TODO check obstacles not from and to
    // TODO trace through block, height slice from z where enters and z where leaves:
    //      Solid: cover (100% for that aim)
    //      Liquid: Density = reduce power
    //      Liquid & gas: range adjusted with parallel flow, miss adjusted with perpendicular flows (also up)
    //      Clouds: obscurement (cant see or miss)
    //      Fire: ignite particle, water extinguish particle (also obstacles)
    static const float weaponHeight = 0.75;
    aimHeight *= to.GetSize().Z();

    auto origin = from.GetPosition()+Position(0,0, from.GetSize().Z() * weaponHeight);  // TODO configure/check/parameterize weapon height from creature & action
    auto aimLow = to.GetPosition() + Position(0,0, aimHeight.begin);

    auto aimMiddle = to.GetPosition() + Position(0, 0, aimHeight.Middle());
    auto targetSurfaces = Facing(origin, aimMiddle);  // TODO size ? of which?
    aimHeight += Engine::Fraction(to.GetPosition().Z());

    // compute normals and dot product with the Vector(aim-origin).Normalized()
    //  keep negative ones (Facing should already have discarded them?). Their sum is 0...-1,
    //  which is proportional to the orthogonal projection of the target on the "vision" of the attacker.
    //  So it's fair to make a weighted sum and just disard any loss as a "bad angle"
    // along the orientation vectors add half the size (or whatever to the bounding box) and iterate, aim at those points
    // then instead of aiming from center mass
    // for taller (upright) targets aim high medium low instead of specific target (headshot/hamstring)
    Engine::Line line(origin.Coord(), aimLow.Coord());
    auto path = line.Voxelize();
    DownPositions(path);

    float progress = 0;
    float cover = 0.0;
    Engine::Range<float> triangle(origin.Z(), origin.Z());
    auto surfaceHeight = aimHeight;
    for(auto it = path.begin(); it!=path.end();++it)
    {
        Slice slice = world.GetMap().GetSlice(Position(it->first.X(), it->first.Y(), triangle.begin), triangle.Size());
        // TODO range = slice.FindGasOpening() (biggest) and if empty slice.FindLiquidOpening()
        auto opening = slice.FindGasOpening();
        if (opening.IsEmpty())
            opening = slice.FindNonSolidOpening();
        surfaceHeight &= opening;
        if (surfaceHeight.IsEmpty()) {
            break;
        }
        progress += it->second;
        auto nextz = lerp(origin.Z(), aimLow.Z(), progress); 
        triangle.begin = lerp(origin.Z(), aimHeight.begin, progress);
        triangle.end = lerp(origin.Z(), aimHeight.end, progress);
    }
    return surfaceHeight.Size();
}

// TODO: should still include map for cover/obscure
double HitChance(const Actor& actor, const Actor& target)
{
    auto hitScore = target.GetStats().Get(Stat::dodge) + target.GetStats().Get(Stat::block);    // TODO: front
    return 1.0; // TODO just always hit for now
}

// TODO also a computation, but still clip to >0 for each.
Computation ComputeMitigation(const Computation& offense, const Computation& defense, std::string_view name)
{
    // Not a full computation (offense-defefense)>=0, to reduce  complexity of the description
    return Computation(std::max(0, (offense - defense).Total()), name);
}

Computation ValueOr(const Computations& computations, Stat::Id key, Computation alternative=Computation())
{
    auto it = computations.find(key);
    if (it==computations.end())
        return alternative;
    else
        return it->second;
}

Computation ComputeDamage(const Computations& offense, const Computations& defense)
{
    Computation result(Engine::Range<int>(0,std::numeric_limits<int>::max()));
    result += ComputeMitigation(ValueOr(offense, Stat::sharp_damage),   ValueOr(defense, Stat::sharp_resist), "sharp");
    result += ComputeMitigation(ValueOr(offense, Stat::blunt_damage),   ValueOr(defense, Stat::blunt_resist), "blunt");
    result += ComputeMitigation(ValueOr(offense, Stat::heat_damage),    ValueOr(defense, Stat::heat_resist), "heat");
    result += ComputeMitigation(ValueOr(offense, Stat::cold_damage),    ValueOr(defense, Stat::cold_resist), "cold");
    result += ComputeMitigation(ValueOr(offense, Stat::lightning_damage), ValueOr(defense, Stat::lightning_resist), "lightning");
    result += ComputeMitigation(ValueOr(offense, Stat::poison_damage),  ValueOr(defense, Stat::poison_resist), "poison");
    result.Simplify();
    return result;
}


}
