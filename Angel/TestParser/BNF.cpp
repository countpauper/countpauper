#include "pch.h"
#include "Parser/BNF.h"
#include "Parser/Parser.h"

namespace Angel::Parser::BNF::Test
{
    class TestBNF : public ::testing::Test
    {
    public:
        static std::any ParseFn(const std::string_view value)
        {
            return std::string(value);
        }

        static std::any Merge(const std::any& a, const std::any& b)
        {
            if (a.has_value())
            {
                if (b.has_value())
                    return std::any_cast<std::string>(a) + "." + std::any_cast<std::string>(b);
                else
                    return a;
            }
            else
                return b;
        }

        class Parser : public BNF::Parser
        {
        public:
            using Tokens = std::map<std::string, const std::string_view>;

  /*
            std::any Parse(const std::string_view rule, const std::any& tokens, const std::string_view value) const override
            {
                Tokens result;

                if (tokens.has_value())
                {
                    const auto& previous = std::any_cast<const Tokens&>(tokens);
                    for (auto& kv : previous)
                    {
                        if (kv.first[0] == '[')
                            result.insert(std::make_pair(std::string(rule) + kv.first, kv.second));
                        else
                            result.insert(std::make_pair(std::string(rule) + "." + kv.first, kv.second));
                    }
                }
                result.insert(std::make_pair(std::string(rule), value));
                return result;
            }

            std::any Merge(const std::any& left, const std::any& right) const override
            {
                if (!left.has_value())
                    return right;
                if (!right.has_value())
                    return left;
                const Tokens& leftI = std::any_cast<const Tokens&>(left);
                const Tokens& rightI = std::any_cast<const Tokens&>(right);
                Tokens merge = leftI;
                merge.insert(rightI.begin(), rightI.end());
                return merge;
            }
            */
        };

        const std::string Get(const Match& m)
        {
            if (m.tokens.has_value())
                return std::any_cast<std::string>(m.tokens);
            else
                return std::string("");
        }

    };

    TEST_F(TestBNF, Nothing)
    {
        Nothing n;
        EXPECT_TRUE(Parse(Rule("nothing", n), "").remaining.empty());
        EXPECT_EQ(Parse(Rule("something", n), "Test").remaining, "Test");
    }

    TEST_F(TestBNF, Literal)
    {
        Literal l("Test");
        EXPECT_TRUE(Parse(Rule("test", l), "Test").remaining.empty());
        EXPECT_THROW(Parse(Rule("fail", l), "Fail"), SyntaxError);
        EXPECT_EQ(Parse(Rule("remaining space", l), "Test ").remaining, " ");
        
        EXPECT_EQ(Get(Parse(Rule( "test", l, ParseFn), "Test")), "Test");

    }

    TEST_F(TestBNF, Whitespace)
    {
        EXPECT_TRUE(Parse(Rule("space", Whitespace()), " ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("multiple space", Whitespace()), "  ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("any whitespace", Whitespace()), "\t\n\r ").remaining.empty());
        EXPECT_EQ(Parse(Rule("after space", Whitespace()), " foo").remaining, "foo");
        EXPECT_THROW(Parse(Rule("no space", Whitespace()), ""), SyntaxError);
        EXPECT_THROW(Parse(Rule("no whitespace", Whitespace()), "Test"), SyntaxError);
        EXPECT_EQ(Parse(Rule("optional whitespace", Whitespace(0)), "Test").remaining, "Test");
        EXPECT_EQ(Parse(Rule("optional no whitespace", Whitespace(0)), "").remaining, "");
    }

    TEST_F(TestBNF, RegularExpression)
    {
        EXPECT_TRUE(Parse(Rule("literal regex", RegularExpression("a")), "a").remaining.empty());
        EXPECT_EQ(Parse(Rule("literal regextra", RegularExpression("b")), "bc").remaining, "c");
        EXPECT_THROW(Parse(Rule("start regex", RegularExpression("b")), "cba"), SyntaxError);
        EXPECT_TRUE(Parse(Rule("regex range", RegularExpression("[a-z]+")), "thequickbrownfoxjumpsoverthelazydog").remaining.empty());
    }

    TEST_F(TestBNF, Disjunction)
    {
        Rule optional{ "optional", Disjunction{ Literal("dog"), Nothing() } };
        EXPECT_TRUE(Parse(optional, "dog").remaining.empty());
        EXPECT_EQ(Parse(optional, "god").remaining, "god");
        EXPECT_THROW(Parse(Rule{ "no option", Disjunction{} }, "god"), SyntaxError);
    }

    TEST_F(TestBNF, Sequence)
    {
        EXPECT_EQ(Parse(Rule{ "empty sequence", Sequence(Merge) }, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single sequence", Sequence{ Merge, Literal("foo") } }, "foobar").remaining, "bar");
        EXPECT_EQ(Get(Parse(Rule{ "merged sequence", Sequence{ Merge, Rule("first",Literal("foo"), ParseFn), Rule("second", Literal("bar"), ParseFn) } }, "foobar")), "foo.bar");
        EXPECT_TRUE(Parse(Rule{ "long sequence", Sequence{ Merge, Literal("foo"), Whitespace(), Literal("bar")} }, "foo bar").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "failed sequence", Sequence{ Merge, Literal("foo"), Whitespace(), Literal("bar")} } , "foobar"), SyntaxError);
    }

    TEST_F(TestBNF, Loop)
    {
        EXPECT_EQ(Parse(Rule{ "empty Loop", Loop(Nothing(), Merge) }, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single Loop", Loop(Literal("f"), Merge) }, "foo").remaining, "oo");
        EXPECT_EQ(Parse(Rule{ "lots Loop", Loop(Literal("f"), Merge) }, "fffffoo").remaining, "oo");
    
        EXPECT_EQ(Get(Parse(Rule( "index", Loop{Rule("part", RegularExpression("[a-z]+."), ParseFn), Merge}), "c bb abc")), "c .bb .abc");
    }

    TEST_F(TestBNF, Ref)
    {
        Rule rule("referred", Literal("foo"), ParseFn );
        EXPECT_TRUE(Parse(Rule{ "ref rule", Ref(rule) }, "foo").remaining.empty());
        EXPECT_EQ(Get(Parse(Rule{ "referee", Ref(rule) }, "foo")), "foo");
    }

    TEST_F(TestBNF, Declare)
    {
        Declare declaration("declared");
        Declare nodeclaration("not declared");
        Rule rule{ "declared", Literal("foo") };

        EXPECT_TRUE(Parse(Rule{ "declared rule", Ref(declaration) }, "foo").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "not declared rule", Ref(nodeclaration) }, "bar"), std::runtime_error);
    }
}