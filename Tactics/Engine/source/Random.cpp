#include "Engine/Random.h"
#include <random>
#include <time.h>

namespace Engine
{

Generator::Generator() :
    Generator(static_cast<unsigned>(time(0)))
{
}

Generator::Generator(unsigned seed) :
    generator(seed),
    seed(seed)
{
    generator.seed(seed);
}

double Generator::Chance()
{
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    return distribution(generator);
}

double Generator::Uniform(const Range<double>& range)
{
    std::uniform_real_distribution<> distribution(range.begin, range.end);
    return distribution(generator);
}


double Generator::Normal(double sigma)
{
    std::normal_distribution<> distribution(0, sigma);
    return distribution(generator);
}

Generator::Seed Generator::GetSeed() const
{
    return seed;
}

void Generator::SetSeed(Seed newSeed)
{
    seed = newSeed;
    generator.seed(seed);
}

Generator::Seed Generator::Store()
{
    std::uniform_int_distribution<unsigned> distribution;
    SetSeed(distribution(generator));
    return GetSeed();
}


void Generator::Restore(Generator::Seed seed)
{
    SetSeed(seed);
}


Generator& Random()
{
    static Generator generator;
    return generator;
}

}