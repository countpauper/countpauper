#include "Game/Utilities.h"
#include "Game/Actor.h"
#include "Game/World.h"
#include "Game/HeightMap.h"
#include "Geometry/Line.h"

namespace Game
{

Orientations Facing(Position from, Position& to)
{
    return Orientations();
}

double ComputeCover(const World& world, const Actor& from, const Actor& to)
{
    // TODO add attack height and target height based on state and size etc
    // TODO check obstacles not from and to
    // TODO trace through block, height slice from z where enters and z where leaves:
    //      SOlid: cover (100% for that aim)
    //      Liquid: Density = reduce power
    //      Liquid & gas: range adjusted with parallel flow, miss adjusted with perpendicular flows (also up)
    //      Clouds: obscurement (cant see or miss)
    //      Fire: ignite particle, water extinguish particle (also obstacles)
    auto origin = from.GetPosition()+Position(0,0, from.Size().z / 2.0);
    auto aim = to.GetPosition() + Position(0,0, to.Size().z / 2.0);

    auto targetSurfaces = Facing(origin, aim);

    // compute normals and dot product with the Vector(aim-origin).Normalized()
    //  keep negative ones (Facing should already have discarded them?). Their sum is 0...-1,
    //  which is proportional to the orthogonal projection of the target on the "vision" of the attacker.
    //  So it's fair to make a weighted sum and just disard any loss as a "bad angle"
    // along the orientation vectors add half the size (or whatever to the bounding box) and iterate, aim at those points
    // then instead of aiming from center mass
    // for taller (upright) targets aim high medium low instead of specific target (headshot/hamstring)
    Engine::Line line(origin.Coord(), aim.Coord());
    auto path = line.Voxelize();
    double progress = 0;
    double cover = 0.0;
    for(const auto& p : path)
    {
        auto z = lerp(origin.Z(), aim.Z(), progress);
        cover = std::max((world.GetMap().GroundHeight(p.first) - z), cover);
        progress += p.second / line.Length();
    }
    return cover;
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
