#include "pch.h"
#include "Parser/BNF.h"

namespace Angel::Parser::BNF::Test
{
    TEST(TestBNF, Nothing)
    {
        Nothing n;
        EXPECT_TRUE(Parse(Rule("nothing", n), "").remaining.empty());
        EXPECT_EQ(Parse(Rule("something", n), "Test").remaining, "Test");
    }

    TEST(TestBNF, Literal)
    {
        Literal l("Test");
        EXPECT_TRUE(Parse(Rule("test", l), "Test").remaining.empty());
        EXPECT_THROW(Parse(Rule("fail", l), "Fail"), SyntaxError);
        EXPECT_EQ(Parse(Rule("remaining space", l), "Test ").remaining, " ");
        
        EXPECT_EQ(Parse(Rule{ "test", l }, "Test")["test"], std::string_view("Test"));

    }

    TEST(TestBNF, Whitespace)
    {
        EXPECT_TRUE(Parse(Rule("space", Whitespace()), " ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("multiple space", Whitespace()), "  ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("any whitespace", Whitespace()), "\t\n\r ").remaining.empty());
        EXPECT_EQ(Parse(Rule("after space", Whitespace()), " foo").remaining, "foo");
        EXPECT_THROW(Parse(Rule("no space", Whitespace()), ""), SyntaxError);
        EXPECT_THROW(Parse(Rule("no whitespace", Whitespace()), "Test"), SyntaxError);
    }

    TEST(TestBNF, RegularExpression)
    {
        EXPECT_TRUE(Parse(Rule("literal regex", RegularExpression("a")), "a").remaining.empty());
        EXPECT_EQ(Parse(Rule("literal regextra", RegularExpression("b")), "bc").remaining, "c");
        EXPECT_THROW(Parse(Rule("start regex", RegularExpression("b")), "cba"), SyntaxError);
        EXPECT_TRUE(Parse(Rule("regex range", RegularExpression("[a-z]+")), "thequickbrownfoxjumpsoverthelazydog").remaining.empty());
    }

    TEST(TestBNF, Disjunction)
    {
        Rule optional{ "optional", Disjunction{ Literal("dog"), Nothing() } };
        EXPECT_TRUE(Parse(optional, "dog").remaining.empty());
        EXPECT_EQ(Parse(optional, "god").remaining, "god");
        EXPECT_THROW(Parse(Rule{ "no option", Disjunction{} }, "god"), SyntaxError);
    }

    TEST(TestBNF, Sequence)
    {
        EXPECT_EQ(Parse(Rule{ "empty sequence", Sequence() }, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single sequence", Sequence{Literal("foo")} }, "foobar").remaining, "bar");
        EXPECT_TRUE(Parse(Rule{ "long sequence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } }, "foo bar").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "faileds equence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } } , "foobar"), SyntaxError);
    }

    TEST(TestBNF, Loop)
    {
        EXPECT_EQ(Parse(Rule{ "empty Loop", Loop(Nothing()) }, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single Loop", Loop(Literal("f")) }, "foo").remaining, "oo");
        EXPECT_EQ(Parse(Rule{ "lots Loop", Loop(Literal("f")) }, "fffffoo").remaining, "oo");
    
        EXPECT_EQ(Parse(Rule{ "index", Loop(RegularExpression("[a-z]+.")) }, "c bb abc")["index[1]"], std::string_view("bb "));
    }

    TEST(TestBNF, Ref)
    {
        Rule rule {"refered", Literal("foo") };
        EXPECT_TRUE(Parse(Rule{ "ref rule", Ref(rule) }, "foo").remaining.empty());

        EXPECT_EQ(Parse(Rule{ "referee", Ref(rule) }, "foo")["referee.refered"], "foo");
    }


}