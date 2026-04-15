#include "Game/Utilities.h"
#include "Game/Actor.h"
#include "Game/World.h"
#include "Game/BlockMap.h"
#include "Game/Slice.h"
#include "Geometry/Line.h"

namespace Game
{

Engine::Orientations Facing(Position from, Position to)
{
    Engine::Orientations result;
    Position V = to - from;
    return Engine::Orientations(Engine::Position{V.X(), V.Y(), round(V.Z())});
}

using ProjectilePath = std::vector<std::pair<Engine::Position, double>>;

void DownPositions(ProjectilePath& path)
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


Position AttackOrigin(const Actor& from)
{
    // TODO configure/check/parameterize weapon height from creature & action
    static const float weaponHeight = 0.75;
    auto origin = from.GetPosition() + Position(0, 0, from.GetSize().Z() * weaponHeight);  
    return origin;
}

Engine::Range<float> AimHeight(const Actor& target)
{
    Engine::Range<float> aimHeight{0,1};
    aimHeight *= static_cast<float>(target.GetSize().Z()); 
    aimHeight += static_cast<float>(target.GetPosition().Z());
    return aimHeight;
}

float HorizontalAttackDistance(Position from, Position to)
{
    auto distance = from.ProjectHorizontal().ManDistance(to.ProjectHorizontal());
    return distance - 1.0f;    // TODO, should not need -1.0 if using origin surface and target surface instead of middle
}





Engine::Range<float> ScanAimWindow(const World& world, const ProjectilePath& path, const Position& origin, Engine::Range<float> aimHeight)
{
    Engine::Range<Position::ZType> triangle(origin.Z(), origin.Z());
    float progress = 0;
    for(auto it =path.begin(); it!=path.end();++it)
    {
        // check when leaving it grid, progress first; 
        progress += it->second; 
        if (progress<1e-3)
            continue;   // don't introduce numeric impression when barely leaving the origin 

        triangle.begin = lerp(origin.Z(), Position::ZType(aimHeight.begin), progress);
        triangle.end = lerp(origin.Z(), Position::ZType(aimHeight.end), progress);
        Slice slice = world.GetMap().GetSlice(Position(it->first.X(), it->first.Y(), triangle.begin), triangle.Size());
        auto opening = slice.FindGasOpening();
        if (opening.IsEmpty())
            opening = slice.FindNonSolidOpening();
        opening /= progress;    // extrapolate to end 
        opening += aimHeight.begin;

        aimHeight &= opening;
        if (aimHeight.IsEmpty()) {
            break;
        }
    }
    return aimHeight;
}

Engine::Range<float> AttackHeight(const World& world, const Actor& from, const Actor& to)
{
    // TODO check obstacles not from and to, this should fix test cover_due_to_obstacle
    //      Use auto targetSurfaces = Facing(origin, aimHeight.Middle()) and sum surfaces instead of just sort of middle plane
    //          compute normals and dot product with the Vector(aim-origin).Normalized()
    //          keep negative ones (Facing should already have discarded them?). Their sum is 0...-1,
    //          which is proportional to the orthogonal projection of the target on the "vision" of the attacker.
    //          So it's fair to make a weighted sum and just disard any loss as a "bad angle"
    //          This should fix partial_cover_due_to_diagonal_height
    //      Subtract shield of targetSurfaces with shield if any
    //      Liquid: Density = reduce power (not for melee?)
    //      Liquid & gas: range adjusted with parallel flow, miss adjusted with perpendicular flows (also up)
    //      Clouds: obscurement (cant see or miss)
    //      Fire: ignite particle, water extinguish particle (also obstacles)
    auto origin = AttackOrigin(from);
    auto aimHeight = AimHeight(to);
   
    auto aimLow = to.GetPosition().ProjectHorizontal() + Position(0,0, aimHeight.begin);
    Engine::Line bottomLine(origin.Coord(), aimLow.Coord());
    auto path = bottomLine.Voxelize();
    DownPositions(path);

    return ScanAimWindow(world, path, origin, aimHeight);
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

unsigned AttackDistance(Position delta)
{
    auto roundDelta = round(delta);
    auto reach = unsigned(std::ceil(roundDelta.Size()*2));
    if (reach>=2)
        return reach-1;
    else
        return reach; 
}

unsigned AttackDistance(const Actor& from, const Actor& to)
{
    auto delta = to.GetPosition() - from.GetPosition();
    return AttackDistance(delta);
}

float VerticalReach(Position delta, unsigned reach)
{
    auto roundDelta = round(delta);
    return (1.0f + reach - roundDelta.Size())/2.0;
}

Engine::Range<float> VerticalReach(const Actor& from, Position to)
{
    float reach = from.GetStats().Get(Stat::reach).Total();
    auto delta = to - from.GetPosition();
    float verticalReach = VerticalReach(delta, reach);
    Engine::Range<float> reachRange(-verticalReach, +verticalReach);
    auto origin = AttackOrigin(from);
    reachRange += static_cast<float>(origin.Z());   // TODO: in FixedPoint as well?    
    return reachRange;
}


}
