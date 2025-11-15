#include "intrusive_list.hpp"
#include <gtest/gtest.h>

namespace eul::Test
{

TEST(intrusive_list, lonely_node)
{
    intrusive_list::node node;
    EXPECT_FALSE(node.linked());
}

TEST(intrusive_list, construct_node)
{
    intrusive_list list;
    intrusive_list::node node(list);
    EXPECT_TRUE(node.linked());
    EXPECT_EQ(list.size(), 1);
}

TEST(intrusive_list, destructed_node_is_erased)
{
    intrusive_list list;
    {
        intrusive_list::node node(list);
        EXPECT_FALSE(list.empty());
    }
    EXPECT_TRUE(list.empty());
}

TEST(intrusive_list, copied_nodes_are_linked)
{
    intrusive_list list;
    intrusive_list::node node(list);
    intrusive_list::node copy(node);
    EXPECT_EQ(list.size(), 2);
    intrusive_list::node assigned;
    assigned = node;
    EXPECT_EQ(list.size(), 3);
    assigned = copy;
    EXPECT_EQ(list.size(), 3);
}

TEST(intrusive_list, moved_nodes_are_linked_old_are_erased)
{
    intrusive_list list;
    intrusive_list::node node(list);
    intrusive_list::node move(std::move(node));
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(node.linked());
    EXPECT_TRUE(move.linked());

    node = std::move(move);
    EXPECT_FALSE(move.linked());
    EXPECT_TRUE(node.linked());
    EXPECT_EQ(list.size(), 1);
}

struct Mixed : intrusive_list::node 
{
    int value;
    Mixed(int v) : value(v) {}
};

TEST(intrusive_list, list_can_be_iterated)
{
    intrusive_list list;
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

TEST(intrusive_list, find)
{
    intrusive_list list;
    intrusive_list::node one(list); 
    intrusive_list::node two(list); 
    intrusive_list::node out;
    
    EXPECT_EQ(list.find(two), list.begin());
    EXPECT_EQ(*list.find(one), one);
    EXPECT_EQ(list.find(out), list.end());
}

TEST(intrusive_list, erase_inside_loop)
{
    Mixed one(1), two(2), three(3);
    intrusive_list list;
    list.push_front(one);
    list.insert(list.push_back(two), three);
    unsigned remaining = 0;
    for(auto& n: list)
    {
        if (static_cast<Mixed&>(n).value==2)
            list.erase(n);
        else 
            ++remaining;
    }     
    EXPECT_EQ(list.size(), remaining);
    EXPECT_FALSE(two.linked());
}

TEST(intrusive_list, clear)
{
    intrusive_list list;
    intrusive_list::node first(list);
    intrusive_list::node second(list);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_FALSE(first.linked());
    EXPECT_FALSE(second.linked());
    list.clear();
}

}
