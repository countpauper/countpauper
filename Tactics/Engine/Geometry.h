namespace Engine
{
    constexpr double PI = 3.14159265358979323846;

    double Rad2Deg(double rad);
    double ShortestTurn(double from, double to);

    double SphereVolume(double radius);


    template<class T> 
    T Lerp(const T& a, const T& b, double factor)
    {   // linear interpolation. Caller should ensure factor is [0...1]
        return (a * (1-factor)) + 
            (b * factor);
    }

    template<typename T>
    T sqr(T v) { return v*v; }
}