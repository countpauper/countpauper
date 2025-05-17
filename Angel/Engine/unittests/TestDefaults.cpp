#include "Engine/Defaults.h"
#include "Logic/Knowledge.h"

#include <gtest/gtest.h>

namespace Angel::Engine::Test
{

TEST(Defaults, Help)
{
    Logic::Knowledge k;
    AddDefaults(k);
    auto help = Logic::to_string(k.Infer(Logic::Predicate("help")));
    EXPECT_TRUE(help.find("help") != help.npos);
}

}
