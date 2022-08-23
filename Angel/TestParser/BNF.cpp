#include "pch.h"
#include "Parser/BNF.h"
#include "Parser/Parser.h"

namespace Angel::Parser::BNF::Test
{
    class TestBNF : public ::testing::Test
    {
    public:
        class Parser : public BNF::Parser
        {
        public:
            using Tokens = std::map<std::string, const std::string_view>;

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
        };

        const std::string_view Get(const Match& m, const std::string_view key)
        {
            const auto& results = std::any_cast<const Parser::Tokens&>(m.tokens);
            return results.at(std::string(key));
        }
        Parser parser;
    };

    TEST_F(TestBNF, Nothing)
    {
        Nothing n;
        EXPECT_TRUE(Parse(Rule("nothing", n), parser, "").remaining.empty());
        EXPECT_EQ(Parse(Rule("something", n), parser, "Test").remaining, "Test");
    }

    TEST_F(TestBNF, Literal)
    {
        Literal l("Test");
        EXPECT_TRUE(Parse(Rule("test", l), parser, "Test").remaining.empty());
        EXPECT_THROW(Parse(Rule("fail", l), parser, "Fail"), SyntaxError);
        EXPECT_EQ(Parse(Rule("remaining space", l), parser, "Test ").remaining, " ");
        
        EXPECT_EQ(Get(Parse(Rule{ "test", l }, parser, "Test"), "test"), std::string_view("Test"));

    }

    TEST_F(TestBNF, Whitespace)
    {
        EXPECT_TRUE(Parse(Rule("space", Whitespace()), parser, " ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("multiple space", Whitespace()), parser, "  ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("any whitespace", Whitespace()), parser, "\t\n\r ").remaining.empty());
        EXPECT_EQ(Parse(Rule("after space", Whitespace()), parser, " foo").remaining, "foo");
        EXPECT_THROW(Parse(Rule("no space", Whitespace()), parser, ""), SyntaxError);
        EXPECT_THROW(Parse(Rule("no whitespace", Whitespace()), parser, "Test"), SyntaxError);
        EXPECT_EQ(Parse(Rule("optional whitespace", Whitespace(0)), parser, "Test").remaining, "Test");
    }

    TEST_F(TestBNF, RegularExpression)
    {
        EXPECT_TRUE(Parse(Rule("literal regex", RegularExpression("a")), parser, "a").remaining.empty());
        EXPECT_EQ(Parse(Rule("literal regextra", RegularExpression("b")), parser, "bc").remaining, "c");
        EXPECT_THROW(Parse(Rule("start regex", RegularExpression("b")), parser, "cba"), SyntaxError);
        EXPECT_TRUE(Parse(Rule("regex range", RegularExpression("[a-z]+")), parser, "thequickbrownfoxjumpsoverthelazydog").remaining.empty());
    }

    TEST_F(TestBNF, Disjunction)
    {
        Rule optional{ "optional", Disjunction{ Literal("dog"), Nothing() } };
        EXPECT_TRUE(Parse(optional, parser, "dog").remaining.empty());
        EXPECT_EQ(Parse(optional, parser, "god").remaining, "god");
        EXPECT_THROW(Parse(Rule{ "no option", Disjunction{} }, parser, "god"), SyntaxError);
    }

    TEST_F(TestBNF, Sequence)
    {
        EXPECT_EQ(Parse(Rule{ "empty sequence", Sequence() }, parser, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single sequence", Sequence{Literal("foo")} }, parser, "foobar").remaining, "bar");
        EXPECT_TRUE(Parse(Rule{ "long sequence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } }, parser, "foo bar").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "faileds equence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } } , parser, "foobar"), SyntaxError);
    }

    TEST_F(TestBNF, Loop)
    {
        EXPECT_EQ(Parse(Rule{ "empty Loop", Loop(Nothing()) }, parser, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single Loop", Loop(Literal("f")) }, parser, "foo").remaining, "oo");
        EXPECT_EQ(Parse(Rule{ "lots Loop", Loop(Literal("f")) }, parser, "fffffoo").remaining, "oo");
    
        EXPECT_EQ(Get(Parse(Rule{ "index", Loop(RegularExpression("[a-z]+.")) }, parser, "c bb abc"),"index[1]"), std::string_view("bb "));
    }

    TEST_F(TestBNF, Ref)
    {
        Rule rule {"referred", Literal("foo") };
        EXPECT_TRUE(Parse(Rule{ "ref rule", Ref(rule) }, parser, "foo").remaining.empty());
        EXPECT_EQ(Get(Parse(Rule{ "referee", Ref(rule) }, parser, "foo"), "referee.referred"), "foo");
    }

    TEST_F(TestBNF, Declare)
    {
        Declare declaration("declared");
        Declare nodeclaration("not declared");
        Rule rule{ "declared", Literal("foo") };

        EXPECT_TRUE(Parse(Rule{ "declared rule", Ref(declaration) }, parser, "foo").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "not declared rule", Ref(nodeclaration) }, parser, "bar"), std::runtime_error);
    }
}