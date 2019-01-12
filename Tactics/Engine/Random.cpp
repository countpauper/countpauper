#include "stdafx.h"
#include "Random.h"
#include <random>
#include <time.h>

namespace Engine
{

Random::Generator::Generator() :
    Generator(static_cast<unsigned>(time(0)))
{
}

Random::Generator::Generator(unsigned seed) :
    std::mt19937(seed)
{
    OutputDebugStringW((std::wstring(L"Random=") + std::to_wstring(seed)).c_str());
}

Random::Generator Random::generator;
double Random::Chance()
{
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    return distribution(generator);
}

double Random::Normal(double sigma)
{
    std::normal_distribution<> distribution(0, sigma);
    return distribution(generator);
}


}