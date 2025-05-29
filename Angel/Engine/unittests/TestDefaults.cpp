#include "Engine/Defaults.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Engine::Test
{

TEST(Defaults, Help)
{
    Logic::Knowledge k;
    AddDefaults(k);
    auto help = k.Infer(Logic::Predicate("help"));
    EXPECT_TRUE(Logic::to_string(help).find("List") != std::string::npos);

    EXPECT_THROW(k.Infer(Logic::Predicate("help", {Logic::Id("cheese")})), std::runtime_error);

    auto details = k.Infer(Logic::Predicate("help", {Logic::Id("help")}));
    EXPECT_TRUE(Logic::to_string(details).find("Describe the function") != std::string::npos);
}

TEST(Defaults, Delete)
{
    Logic::Knowledge k { 
        Logic::Predicate("evil"),
        Logic::Predicate("good")
    };
    AddDefaults(k);
    EXPECT_EQ(k.Infer(Logic::Predicate("delete", {Logic::Id("evil")})), Logic::Integer(1));
    EXPECT_FALSE(k.Knows(Logic::Predicate("evil")));
    EXPECT_TRUE(k.Knows(Logic::Predicate("good")));
}

TEST(Defaults, Print)
{
    Logic::Knowledge k;
    AddDefaults(k);
    EXPECT_EQ(k.Infer(Logic::Predicate("print", {Logic::String("Hello world!")})), Logic::Boolean(true));
}

}
