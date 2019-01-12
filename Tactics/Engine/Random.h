#include <random>

namespace Engine
{
    class RandomGenerator
    {
    public:
        RandomGenerator();
        RandomGenerator(unsigned seed);
        double Chance();
        double Normal(double sigma); 
        unsigned Seed() const;
        void SetSeed(unsigned newSeed);
    private:
        unsigned seed;
        std::mt19937 generator;
    };
    static RandomGenerator Random;
}