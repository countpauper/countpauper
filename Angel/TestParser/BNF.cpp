#include "pch.h"
#include "Parser/BNF.h"
#include "Parser/Interpreter.h"

namespace Angel::Parser::BNF::Test
{
    class TestBNF : public ::testing::Test
    {
    public:
        class Interpreter : public BNF::Interpreter
        {
        public:
            using Interpretation = std::map<std::string, const std::string_view>;

            std::any Interpret(const std::string_view rule, const std::any& interpretation, const std::string_view value) const override
            {
                Interpretation result;

                if (interpretation.has_value())
                {
                    const auto& previous = std::any_cast<const Interpretation&>(interpretation);
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
                const Interpretation& leftI = std::any_cast<const Interpretation&>(left);
                const Interpretation& rightI = std::any_cast<const Interpretation&>(right);
                Interpretation merge = leftI;
                merge.insert(rightI.begin(), rightI.end());
                return merge;
            }


        };

        const std::string_view Get(const Match& m, const std::string_view key)
        {
            const auto& results = std::any_cast<const Interpreter::Interpretation&>(m.interpretation);
            return results.at(std::string(key));
        }
        Interpreter interpreter;
    };

    TEST_F(TestBNF, Nothing)
    {
        Nothing n;
        EXPECT_TRUE(Parse(Rule("nothing", n), interpreter, "").remaining.empty());
        EXPECT_EQ(Parse(Rule("something", n), interpreter, "Test").remaining, "Test");
    }

    TEST_F(TestBNF, Literal)
    {
        Literal l("Test");
        EXPECT_TRUE(Parse(Rule("test", l), interpreter, "Test").remaining.empty());
        EXPECT_THROW(Parse(Rule("fail", l), interpreter, "Fail"), SyntaxError);
        EXPECT_EQ(Parse(Rule("remaining space", l), interpreter, "Test ").remaining, " ");
        
        EXPECT_EQ(Get(Parse(Rule{ "test", l }, interpreter, "Test"), "test"), std::string_view("Test"));

    }

    TEST_F(TestBNF, Whitespace)
    {
        EXPECT_TRUE(Parse(Rule("space", Whitespace()), interpreter, " ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("multiple space", Whitespace()), interpreter, "  ").remaining.empty());
        EXPECT_TRUE(Parse(Rule("any whitespace", Whitespace()), interpreter, "\t\n\r ").remaining.empty());
        EXPECT_EQ(Parse(Rule("after space", Whitespace()), interpreter, " foo").remaining, "foo");
        EXPECT_THROW(Parse(Rule("no space", Whitespace()), interpreter, ""), SyntaxError);
        EXPECT_THROW(Parse(Rule("no whitespace", Whitespace()), interpreter, "Test"), SyntaxError);
    }

    TEST_F(TestBNF, RegularExpression)
    {
        EXPECT_TRUE(Parse(Rule("literal regex", RegularExpression("a")), interpreter, "a").remaining.empty());
        EXPECT_EQ(Parse(Rule("literal regextra", RegularExpression("b")), interpreter, "bc").remaining, "c");
        EXPECT_THROW(Parse(Rule("start regex", RegularExpression("b")), interpreter, "cba"), SyntaxError);
        EXPECT_TRUE(Parse(Rule("regex range", RegularExpression("[a-z]+")), interpreter, "thequickbrownfoxjumpsoverthelazydog").remaining.empty());
    }

    TEST_F(TestBNF, Disjunction)
    {
        Rule optional{ "optional", Disjunction{ Literal("dog"), Nothing() } };
        EXPECT_TRUE(Parse(optional, interpreter, "dog").remaining.empty());
        EXPECT_EQ(Parse(optional, interpreter, "god").remaining, "god");
        EXPECT_THROW(Parse(Rule{ "no option", Disjunction{} }, interpreter, "god"), SyntaxError);
    }

    TEST_F(TestBNF, Sequence)
    {
        EXPECT_EQ(Parse(Rule{ "empty sequence", Sequence() }, interpreter, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single sequence", Sequence{Literal("foo")} }, interpreter, "foobar").remaining, "bar");
        EXPECT_TRUE(Parse(Rule{ "long sequence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } }, interpreter, "foo bar").remaining.empty());
        EXPECT_THROW(Parse(Rule{ "faileds equence", Sequence{ Literal("foo"), Whitespace(), Literal("bar") } } , interpreter, "foobar"), SyntaxError);
    }

    TEST_F(TestBNF, Loop)
    {
        EXPECT_EQ(Parse(Rule{ "empty Loop", Loop(Nothing()) }, interpreter, "foo").remaining, "foo");
        EXPECT_EQ(Parse(Rule{ "single Loop", Loop(Literal("f")) }, interpreter, "foo").remaining, "oo");
        EXPECT_EQ(Parse(Rule{ "lots Loop", Loop(Literal("f")) }, interpreter, "fffffoo").remaining, "oo");
    
        EXPECT_EQ(Get(Parse(Rule{ "index", Loop(RegularExpression("[a-z]+.")) }, interpreter, "c bb abc"),"index[1]"), std::string_view("bb "));
    }

    TEST_F(TestBNF, Ref)
    {
        Rule rule {"referred", Literal("foo") };
        EXPECT_TRUE(Parse(Rule{ "ref rule", Ref(rule) }, interpreter, "foo").remaining.empty());
        EXPECT_EQ(Get(Parse(Rule{ "referee", Ref(rule) }, interpreter, "foo"), "referee.referred"), "foo");
    }


}