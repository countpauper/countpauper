#include "format.hpp"
#include <string>
#include <cmath>

namespace eul 
{

static constexpr int MAX_SIGNIFICANT_DIGITS=6;
static constexpr int MIN_SIGNIFICANT_DIGITS = MAX_SIGNIFICANT_DIGITS-3;
static constexpr int BUFFER_SIZE = MAX_SIGNIFICANT_DIGITS + 3 + 4;    // 0, period and 4 max exponent e-38  -

int compute_period_location(float value)
{
    value = fabs(value);
    return std::floor(std::log10(value));
}

unsigned compute_integer_fraction(float fraction, unsigned digits)
{
    float fraction_multiplier = std::pow(10.0, digits);
    return std::round(fabs(fraction) * fraction_multiplier);
}

const char* numeric_float_to_string(float value)
{
    // NB: buffer is not safe for concurrent access. Protect if used in production.
    static char buffer[BUFFER_SIZE];   
    int period_location = compute_period_location(value);
    if (period_location >= MAX_SIGNIFICANT_DIGITS ) 
    {
        unsigned exponent=period_location;
        value /= std::pow(10.0f, exponent);
        snprintf(buffer, sizeof(buffer), "%se%u", numeric_float_to_string(value), exponent);
        return buffer;
    }
    else if (period_location <= -MIN_SIGNIFICANT_DIGITS-1) 
    {
        unsigned neg_exponent = -period_location;
        value *= std::pow(10.0f, neg_exponent);
        snprintf(buffer, sizeof(buffer), "%se-%u", numeric_float_to_string(value), neg_exponent);
        return buffer;
    }
    if (period_location>=0) 
    {
        int fraction_digits = MAX_SIGNIFICANT_DIGITS - period_location - 1;
        int whole = value;
        unsigned remainder = compute_integer_fraction(value - whole, fraction_digits);
        snprintf(buffer, sizeof(buffer), "%d.%0*u", (int)value, fraction_digits, remainder);
    } 
    else 
    {
        int fraction_digits = MAX_SIGNIFICANT_DIGITS - 1;
        unsigned remainder = compute_integer_fraction(value, fraction_digits);
        if (value>=0) 
            snprintf(buffer, sizeof(buffer), "0.%0*d", fraction_digits, remainder);
        else
            snprintf(buffer, sizeof(buffer), "-0.%0*d", fraction_digits, remainder);
    }
    return buffer;
}
const char* to_string(float value)
{
    if (value==0.0) 
        return "0.0";
    else if (std::isnan(value)) 
        return "NaN";
    else if (std::isinf(value))
        return (value>=0) ? "INF" : "-INF";
    else
        return numeric_float_to_string(value);
}

}
