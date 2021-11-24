#pragma once

#define EXPECT_3D_EQ(ref, target) \
{ \
    const auto& _r=(ref); \
    const auto& _t = (target); \
    EXPECT_DOUBLE_EQ(_r.x, _t.x); \
    EXPECT_DOUBLE_EQ(_r.y, _t.y); \
    EXPECT_DOUBLE_EQ(_r.z, _t.z); \
 }


#define EXPECT_3D_NEAR(ref, target, abs_error) \
{ \
    const auto& _r=(ref); \
    const auto& _t = (target); \
    EXPECT_NEAR(_r.x, _t.x, abs_error); \
    EXPECT_NEAR(_r.y, _t.y, abs_error); \
    EXPECT_NEAR(_r.z, _t.z, abs_error); \
 }
