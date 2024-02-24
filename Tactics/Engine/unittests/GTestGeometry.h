#pragma once

#define EXPECT_3D_EQ(v1, v2) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_DOUBLE_EQ(_real.x, _expected.x) << "X"; \
    EXPECT_DOUBLE_EQ(_real.y, _expected.y) << "Y"; \
    EXPECT_DOUBLE_EQ(_real.z, _expected.z) << "Z"; \
 }

#define EXPECT_3D_NEAR(v1, v2, abs_error) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_NEAR(_real.x, _expected.x, abs_error) << "X"; \
    EXPECT_NEAR(_real.y, _expected.y, abs_error) << "Y"; \
    EXPECT_NEAR(_real.z, _expected.z, abs_error) << "Z"; \
 }

#define EXPECT_MATRIX_NEAR(m1, m2, abs_error) \
{ \
    for(int r=0; r<4; ++r) \
        for(int c=0; c<4; ++c) \
            EXPECT_NEAR(m1[r][c], m2[r][c], abs_error) << "[" << r << "," << c << "]"; \
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
