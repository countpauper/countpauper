#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Utility/Singleton.h"

namespace Engine::test
{

class TestClass
{
public:
    TestClass() = default;
    TestClass(bool init) : member(init) {}

    bool member = false;
};

TEST(Singleton, TwoSingletonsAreTheSame)
{
    Singleton<TestClass>::Reset();
    Singleton<TestClass> a;
    Singleton<TestClass> b;
    EXPECT_EQ(&*a, &*b);
}

TEST(Singleton, CanCreateSeparateInstance)
{
    Singleton<TestClass>::Reset();
    Singleton<TestClass> singleton;
    TestClass other;

    singleton->member = true;
    EXPECT_FALSE(other.member);
}

TEST(Singleton, CopySingleton)
{
    Singleton<TestClass>::Reset();
    Singleton<TestClass> singleton;
    Singleton<TestClass> other(singleton);
    EXPECT_EQ(&*singleton, &*other);
}

TEST(Singleton, SingletonStaticGetter)
{
    Singleton<TestClass>::Reset();
    Singleton<TestClass> singleton;
    EXPECT_EQ(&Singleton<TestClass>::Get(), &*singleton);
}

TEST(Singleton, ParameterizeSingleton)
{
    Singleton<TestClass>::Reset();
    Singleton<TestClass> initialized(true);
    EXPECT_TRUE(Singleton<TestClass>::Get().member);
}

}
