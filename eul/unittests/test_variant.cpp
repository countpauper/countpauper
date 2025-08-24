#include <gtest/gtest.h>
#include "variant.hpp"

namespace eul::Test
{

struct AlternativeA 
{
    AlternativeA() = default;
    AlternativeA(char c) : alt(c) {}
    bool operator==(const AlternativeA& rhs) const { return rhs.alt == alt; }
    char alt='A';
};

struct AlternativeB 
{
    AlternativeB() = default;
    AlternativeB(char c) : alt(c) {}
    bool operator==(const AlternativeB& rhs) const { return rhs.alt == alt; }
    char alt='B';
};

using Variant0AB = std::variant<std::monostate, AlternativeA, AlternativeB>;

TEST(variant, overloaded_visit)
{
    auto test_visit = [](const Variant0AB& var) -> char
    {
        return std::visit(overloaded_visit{
            [](const AlternativeA& A) { return A.alt; },
            [](const AlternativeB&) { return 'B'; },
            [](std::monostate) { return '?'; }
        }, var);
    };

    EXPECT_EQ(test_visit(AlternativeA()), 'A');
    EXPECT_EQ(test_visit(AlternativeB()), 'B');
    EXPECT_EQ(test_visit(Variant0AB()), '?');
}

TEST(variant, variant_index_of)
{
    static_assert(variant_index_of<AlternativeA, Variant0AB>() == 1);
    static_assert(variant_index_of<AlternativeB, Variant0AB>() == 2);
    static_assert(variant_index_of<std::monostate, Variant0AB>() == 0);
}

TEST(variant, construct_by_index)
{
    EXPECT_TRUE(holds_alternative<AlternativeA>(construct_variant_by_index<Variant0AB>(1).value()));
    static_assert(holds_alternative<AlternativeB>(construct_variant_by_index<Variant0AB>(2).value()));
    static_assert(!construct_variant_by_index<Variant0AB>(3));
}

// construct_variant_by_index with arguments only works if all variants have the same constructor signature 
// this precludes std::monostate
using VariantAB = std::variant<AlternativeA, AlternativeB>;
TEST(variant, construct_by_index_with_args)
{
    EXPECT_EQ(std::get<AlternativeA>(construct_variant_by_index<VariantAB>(0, 'a').value()), AlternativeA('a'));
}


}
