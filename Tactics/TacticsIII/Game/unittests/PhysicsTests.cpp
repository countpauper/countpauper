#include "Game/Map.h"
#include "Game/Mock/MockMap.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>




namespace Game::Test
{
using namespace ::testing;

TEST(Physiscs, Flow)
{
    Map map(Engine::Size{2,1,1});
    // TODO fill with water and air and watch the water flow
    // this would be because the pressure in the water (by its own weight) causing flow 
    // Then the flow moves material 
    
}

}