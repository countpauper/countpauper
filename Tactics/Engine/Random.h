#include <random>

namespace Engine
{
    class Random
    {
    public:
        class Generator : public std::mt19937
        {
        public:
            Generator();
            Generator(unsigned seed);
        };
        static double Chance();
        static double Normal(double sigma);
    private:
        static Generator generator;
    };
}