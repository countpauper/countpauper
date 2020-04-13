#include "pch.h"
#include <array>
#include <numeric>
#include <iostream>
#include "Random.h"

namespace Engine::Test
{

template<unsigned N>
double minimum(std::array<double, N>& values)
{
    return std::accumulate(values.begin(), values.end(), std::numeric_limits<double>::infinity(), [](double m, double v)
    {
        return std::min(m, v);
    });
}

template<unsigned N>
double maximum(std::array<double, N>& values)
{
    return std::accumulate(values.begin(), values.end(), -std::numeric_limits<double>::infinity(), [](double m, double v)
    {
        return std::max(m, v);
    });
}

template<unsigned N>
double average(std::array<double, N>& values)
{
    double sum = std::accumulate(values.begin(), values.end(), 0.0, [](double a, double v)
    {
        return a + v;
    });
    return sum / static_cast<double>(N);
}

template<unsigned N>
double stddev(std::array<double, N>& values)
{
    double avg = average(values);
    double dev = std::accumulate(values.begin(), values.end(), 0.0, [avg](double d, double v)
    {
        return d + (v - avg)*(v - avg);
    });
    return std::sqrt(dev / static_cast<double>(N));
}


TEST(Random, Chance)
{
    std::array<double, 100000> values;
    for (auto& v : values)
    {
        v = Engine::Random().Chance();
    }
    // tested for 10000 iterations, but random gonna random
    EXPECT_LE(maximum(values), 1.0);
    EXPECT_GE(minimum(values), 0.0);
    EXPECT_NEAR(average(values), 0.5, 1);
}

TEST(Random, Normal)
{
    std::array<double, 100000> values;
    for (auto& v : values)
    {
        v = Engine::Random().Normal(1.5);
    }
    // tested for 10000 iterations (and raised tolerances), but random gonna random
    EXPECT_NEAR(average(values), 0, 0.025);
    EXPECT_NEAR(stddev(values), 1.5, 2);
}


TEST(Random, Seed)
{
    Engine::Random().SetSeed(Engine::Random().GetSeed());
    std::array<double, 1000> original;
    for (auto& v : original)
    {
        v = Engine::Random().Chance();
    }
    Engine::Random().SetSeed(Engine::Random().GetSeed());
    for (const auto& v : original)
    {
        EXPECT_EQ(v, Engine::Random().Chance());
    }
}

TEST(Random, Store)
{
    auto state = Engine::Random().Store();
    std::array<double, 1000> original;
    for (auto& v : original)
    {
        v = Engine::Random().Chance();
    }
    Engine::Random().Restore(state);
    for (const auto& v : original)
    {
        EXPECT_EQ(v, Engine::Random().Chance());
    }
}

}

