#include <gtest/gtest.h>
#include "Geometry/Angles.h"
#include "Geometry/Quaternion.h"

namespace Engine::Test
{

const double maxError = 0.001;

TEST(Angles, Degrees)
{
    EXPECT_EQ(Engine::Rad2Deg(0), 0);
    EXPECT_NEAR(Engine::Rad2Deg(Engine::PI), 180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(-Engine::PI), -180, maxError);
    EXPECT_NEAR(Engine::Rad2Deg(2 * Engine::PI), 360, maxError);
}


TEST(Angles, Turn)
{
    double halfpi = Engine::PI * 0.5;
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0, halfpi), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, 0), -halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(halfpi, Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(0.75*Engine::PI, -0.75*Engine::PI), halfpi, maxError);
    EXPECT_NEAR(Engine::ShortestTurn(-0.75*Engine::PI, 0.75*Engine::PI), -halfpi, maxError);
}

TEST(Angles, FaceYawPitch)
{
    Vector forward{0,1,0};
    EXPECT_EQ(FaceYawPitch(forward), std::make_pair(0.0, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector( 1, 0, 0)), std::make_pair(PI/2, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector(-1, 0, 0)), std::make_pair(-PI/2, 0.0));
    EXPECT_EQ(FaceYawPitch(Vector( 0, 0, 1)), std::make_pair(0.0, PI/2));

    EXPECT_EQ(FaceYawPitch(Quaternion(Vector(0,0,1), 1.0) * forward), std::make_pair(-1.0, 0.0));
    EXPECT_EQ(FaceYawPitch(Quaternion(Vector(1,0,0), -0.5) * forward), std::make_pair(0.0, -0.5));
    auto yawPitchZAxis = FaceYawPitch(Quaternion(Vector(0,0,1), -0.5) * Quaternion(Vector(1,0,0), 1.0) * forward);
    EXPECT_DOUBLE_EQ(yawPitchZAxis.first, 0.5);
    EXPECT_DOUBLE_EQ(yawPitchZAxis.second, 1.0);
/*  // TODO no idea why the sign of these angles doesn't match or why yaw is inverted but pitch is not in the tests above
    auto yawPitchYAxis = FaceYawPitch(Quaternion(Vector(0,1,0), -0.5) * Quaternion(Vector(1,0,0), 1.0) * Vector(0, 0, 1), false);
    EXPECT_DOUBLE_EQ(yawPitchYAxis.first, -0.5);
    EXPECT_DOUBLE_EQ(yawPitchYAxis.second, -1.0);
*/
}


}
