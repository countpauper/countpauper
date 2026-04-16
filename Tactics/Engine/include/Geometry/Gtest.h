#pragma once

#define EXPECT_3D_EQ(v1, v2) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_DOUBLE_EQ(_real.X(), _expected.X()) << "X"; \
    EXPECT_DOUBLE_EQ(_real.Y(), _expected.Y()) << "Y"; \
    EXPECT_DOUBLE_EQ(_real.Z(), _expected.Z()) << "Z"; \
 }

#define EXPECT_3D_NEAR(v1, v2, abs_error) \
{ \
    const auto& _real=(v1); \
    const auto& _expected = (v2); \
    EXPECT_NEAR(_real.X(), _expected.X(), abs_error) << "X"; \
    EXPECT_NEAR(_real.Y(), _expected.Y(), abs_error) << "Y"; \
    EXPECT_NEAR(_real.Z(), _expected.Z(), abs_error) << "Z"; \
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
    EXPECT_DOUBLE_GE(_real.X(), _compare.X()); \
    EXPECT_DOUBLE_GE(_real.Y(), _compare.Y(); \
    EXPECT_DOUBLE_GE(_real.Z(), _compare.Z(); \
 }

#define EXPECT_3D_LE(ref, target) \
{ \
    const auto& _real=(v1); \
    const auto& _compare = (v2); \
    EXPECT_DOUBLE_LE(_real.X(), _compare.Z()); \
    EXPECT_DOUBLE_LE(_real.Y(), _compare.Y()); \
    EXPECT_DOUBLE_LE(_real.Z(), _compare.Z()); \
 }

#define EXPECT_RANGE_EQ(r1, r2) \
{ \
    const auto& _real=(r1); \
    const auto& _expected = (r2); \
    EXPECT_DOUBLE_EQ(_real.begin, _expected.begin) << "begin"; \
    EXPECT_DOUBLE_EQ(_real.end, _expected.end) << "end"; \
}

#define EXPECT_RANGE_NEAR(r1, r2, abs_error) \
{ \
    const auto& _real=(r1); \
    const auto& _expected = (r2); \
    EXPECT_NEAR(static_cast<double>(_real.begin), static_cast<double>(_expected.begin), static_cast<double>(abs_error)) << "begin"; \
    EXPECT_NEAR(static_cast<double>(_real.end), static_cast<double>(_expected.end), static_cast<double>(abs_error)) << "end"; \
}
