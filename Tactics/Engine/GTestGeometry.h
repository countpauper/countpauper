#pragma once

#define EXPECT_3D_EQ(v1, v2) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_DOUBLE_EQ(_real.x, _expected.x); \
    EXPECT_DOUBLE_EQ(_real.y, _expected.y); \
    EXPECT_DOUBLE_EQ(_real.z, _expected.z); \
 }


#define EXPECT_3D_NEAR(v1, v2, abs_error) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_NEAR(_real.x, _expected.x, abs_error); \
    EXPECT_NEAR(_real.y, _expected.y, abs_error); \
    EXPECT_NEAR(_real.z, _expected.z, abs_error); \
 }


#define EXPECT_3D_GE(v1, v2) \
{ \
    const auto& _real=(v1); \
    const auto& _compare = (v2); \
    EXPECT_DOUBLE_GE(_real.x, _compare.x); \
    EXPECT_DOUBLE_GE(_real.y, _compare.y); \
    EXPECT_DOUBLE_GE(_real.z, _compare.z); \
 }

#define EXPECT_3D_LE(ref, target) \
{ \
    const auto& _real=(v1); \
    const auto& _compare = (v2); \
    EXPECT_DOUBLE_LE(_real.x, _compare.x); \
    EXPECT_DOUBLE_LE(_real.y, _compare.y); \
    EXPECT_DOUBLE_LE(_real.z, _compare.z); \
 }
