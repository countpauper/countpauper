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

}
