#include "stdafx.h"
#include "Random.h"
#include <random>
#include <time.h>

namespace Engine
{

RandomGenerator::RandomGenerator() :
    RandomGenerator(static_cast<unsigned>(time(0)))
{
}

RandomGenerator::RandomGenerator(unsigned seed) :
    generator(seed),
    seed(seed)
{
    generator.seed(seed);
}

double RandomGenerator::Chance()
{
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    return distribution(generator);
}

double RandomGenerator::Normal(double sigma)
{
    std::normal_distribution<> distribution(0, sigma);
    return distribution(generator);
}

unsigned RandomGenerator::Seed() const
{
    return seed;
}

void RandomGenerator::SetSeed(unsigned newSeed)
{
    seed = newSeed;
    generator.seed(seed);
}

}