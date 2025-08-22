#include "intrusive_list.hpp"
#include <gtest/gtest.h>

namespace eul::Test
{

TEST(List, FreeNode)
{
    LinkedList::Node node;
    EXPECT_FALSE(node.IsLinked());
}

TEST(List, ConstructedNode)
{
    LinkedList list;
    LinkedList::Node node(list);
    EXPECT_TRUE(node.IsLinked());
    EXPECT_EQ(list.size(), 1);
}

TEST(List, DestructedNodeIsErased)
{
    LinkedList list;
    {
        LinkedList::Node node(list);
        EXPECT_FALSE(list.empty());
    }
    EXPECT_TRUE(list.empty());
}

TEST(List, CopiedNodesAreAdded)
{
    LinkedList list;
    LinkedList::Node node(list);
    LinkedList::Node copy(node);
    EXPECT_EQ(list.size(), 2);
    LinkedList::Node assigned;
    assigned = node;
    EXPECT_EQ(list.size(), 3);
    assigned = copy;
    EXPECT_EQ(list.size(), 3);
}

TEST(List, MovedNodesAreAddedOldAreRemoved)
{
    LinkedList list;
    LinkedList::Node node(list);
    LinkedList::Node move(std::move(node));
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(node.IsLinked());
    EXPECT_TRUE(move.IsLinked());

    node = std::move(move);
    EXPECT_FALSE(move.IsLinked());
    EXPECT_TRUE(node.IsLinked());
    EXPECT_EQ(list.size(), 1);
}

struct Mixed : LinkedList::Node 
{
    int value;
    Mixed(int v) : value(v) {}
};

TEST(List, ListCanBeIterated)
{
    LinkedList list;
    Mixed one(1), two(2), three(3);
    list.push_front(one);
    list.push_front(two);
    list.push_front(three);
    int sum = 0;
    for(auto& n: list)
    {
        sum+=static_cast<Mixed&>(n).value;
    }     
    EXPECT_EQ(sum, 6);
}

TEST(List, EraseInLoop)
{
    Mixed one(1), two(2), three(3);
    LinkedList list;
    list.push_front(one);
    list.insert(list.push_back(two), three);
    for(auto& n: list)
    {
        if (static_cast<Mixed&>(n).value==2)
            list.erase(n);
    }     
    EXPECT_FALSE(two.IsLinked());
}

TEST(List, Clear)
{
    LinkedList list;
    LinkedList::Node first(list);
    LinkedList::Node second(list);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_FALSE(first.IsLinked());
    EXPECT_FALSE(second.IsLinked());
    list.clear();
}

}
