#include <boost/test/included/unit_test.hpp>

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
    boost::unit_test::framework::master_test_suite().p_name.value = "Game Unit Test";
    return 0;
}