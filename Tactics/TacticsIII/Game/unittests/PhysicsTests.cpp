#include "Game/Map.h"
#include "Game/Physics.h"
#include "Game/Mock/MockMap.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>




namespace Game::Test
{
using namespace ::testing;


// TODO: 
// [done] Gravity: Layer flow increase downwards (if nothing around)
// [done] Dampening: internal viscosity slows down flows and sets it to 0 for solids (TODO they can still fall if there's nothing below?)
// Obstruction: layer flow (downwards?) is reduced (to 0) if neighbouring materials (especially solids) and low granulartity (of both) (so earth still falls mostly) (also water over riverbed later)
// Presure: Layers trying to flow downwards increase pressure in layers below and inside themselves if they can't flow down (air on water water on rock)
// Relaxation: the weight of a cell creates "pressure" on the neighbour below. If it is compressible its volume decreases. If the layer itself is compressible its volume increases
// Falling: If the weight of a layer compresses a layer below, but it's incompressible, then it falls. Instead the neighbour above grows and so on. 
// (Hydro)static pressure: If the . (always for non solids) it causes a pressure gradient (P=pgh that is applied horizontally depending on touching area height range. 
// Surface tension (diminish pressure difference between neighbours that are liquid, set to 0 at solids)
// Equilibrium: water layers pressure is set on creation depending on depth (so it flows sideways) but doesn't flow up in an equilibrium. (Perhaps water layers should be at most 1-2 meters?) 
// Falling pressure: rock falling down in air creates very high pressure mini layer to a limit that offsets the gravity, until it escapes (minimum size 1?)
// Sink: rock/earth falling in water (or high pressure air) creates pressure that slows sinking
// Pressure flow: differences in pressure between neighbouring layers creates sideways flow 
// Expand: High pressure layers flow up (or down) into lower pressure vertical neighbours until they even out. THis is like flowing up and immediately fusing 
// Merge: Vertical layers that (have landed/flowed) unto similar neighbours are merged, averaging temperature, pressure and flow (if within limits)   
// Viscosity: More viscous materials reduce the flow as internal friction
// Flow: If after computing and evening out gravity, friction and pressure, there is still flow, materials are exchanged between layers of the same material 
// Leak: Material leaks into neighbouring layers with less pressure but different material, creating new layers 
// Shore: Flow is stopped by neighbours with surface hardness (this may already be needed sooner for falling pressure?)
// Edge: Neighbouring layers are extended outside map edge for determining friction and flow (what about pressure?)
// Source: Edge source can flow material into layer (to create river/wind/clouds) 

// Later more advanced (bigger) topics, yet to be broken down 
// Friction: dampen flow along neighbouring solids with a mu_friction, which could also be used for gameplay. 
// Shear stress: friction with liquids/gas,  see viscotiy): 
// Partial obstruction: Flowing into multiple neighbours: one deformable and the other hard, will redirect the flow into the deformable one but at a greater speed? 
// Collision: layers moving hard into each other can cause granular break down + clouds and surfaces (non elastic) or even flow reversal? (can flow be that fast? runge kutta gravity and velocity over time)
// Temperature: Increases pressure, diffuses, spreads with flow, state changes: ice Friction is lower, hardness higher etc  
// Erosion: Flow over low granular material can create clouds
// Surfaces: Thin layers dissolve to puddles and big surfaces turn to layers 
// Clouds: liquids evaporate and condense, move with the flow 
// Charge: Lighting and charge moves and spreads 
TEST(Physics, Gravity)
{
    Map map(Engine::Size(1,1,1), {{Material::stone, 0.5}});
    Gravity gravity(9.80665f);
    auto& slice = map[0,0];
    gravity(1.0f, slice);
    ASSERT_EQ(slice[1].material, Material::air);
    EXPECT_GE(slice[1].GetFlow(Orientation::down), 0.5);
    EXPECT_TRUE(std::ranges::all_of(Orientations::horizontal, [&slice](Engine::Orientation ori) { return slice[1].GetFlow(ori) == 0.0  ; }));
}


TEST(Physics, Viscosity)
{
    Map map(Engine::Size(1,1,1), {{Material::stone, 0.5}});
    Gravity gravity(9.80665f);
    Viscosity viscosity;
    auto& slice = map[0,0];
    gravity(1.0f, slice);
    ASSERT_EQ(slice[1].material, Material::air);
    
    auto gravityFlow = slice[1].GetFlow(Orientation::down);
    viscosity(1.0f, slice);
    EXPECT_LT(slice[1].GetFlow(Orientation::down), gravityFlow);

    ASSERT_EQ(slice[0].material, Material::stone);
    EXPECT_EQ(slice[0].GetFlow(Orientation::down), 0.0);
}


TEST(Physics, Advection)
{
    Map map(Engine::Size{2,1,1}, {{Material::water, 1.0}, {Material::stone, 0.0}});

    // TODO Make water flow to the right.
    // this would be because the pressure different in the water (by its own weight) causing  
    // Then the flow moves material 

}

}