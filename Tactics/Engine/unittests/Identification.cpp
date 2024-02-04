#include <gtest/gtest.h>
#include "Engine/Identification.h"

namespace Engine::Test
{

    TEST(Identification, Create)
    {
        Identification a;
        Identification b;
        EXPECT_EQ(Identification::FindId(0), nullptr);
        EXPECT_EQ(Identification::FindId(a.Id()), &a);
        EXPECT_EQ(Identification::FindId(b.Id()), &b);
    }

    TEST(Identification, Release)
    {
        auto dummyId = 0;
        {
            Identification dummy;
            dummyId = dummy.Id();
        }
        EXPECT_EQ(Identification::FindId(dummyId), nullptr);
        Identification a;
        EXPECT_EQ(Identification::FindId(dummyId), nullptr);
        EXPECT_EQ(Identification::FindId(a.Id()), &a);
    }

    TEST(Identification, Copy)
    {
        Identification a;
        auto b = a;
        EXPECT_EQ(Identification::FindId(a.Id()), &a);
        EXPECT_EQ(Identification::FindId(b.Id()), &b);
    }

    TEST(Identification, Move)
    {
        Identification a;
        auto b = std::move(a);
        EXPECT_EQ(Identification::FindId(a.Id()), nullptr);
        EXPECT_EQ(Identification::FindId(b.Id()), &b);
    }
}
