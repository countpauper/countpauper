namespace Angel::Logic
{

class Subset : public Comparison<Subset>
{
public:
    using Comparison<In>::Comparison;
    constexpr static BinaryOperator ope{L'∈'};
};

}