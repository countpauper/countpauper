namespace Engine
{
    constexpr double PI = 3.14159265358979323846;

    double Rad2Deg(double rad);
    double ShortestTurn(double from, double to);

    double SphereVolume(double radius);

    template<typename T>
    T sqr(T v) { return v*v; }
}