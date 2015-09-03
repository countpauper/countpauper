// Input: 7 segment display, bits = segment on, hexadecimal data set
// ---0 ---
// |      |
// 1      2
// |      |
// ---3----
// |      |
// 4      5
// |      |
// ---6----
// Output: binary value 0000 to 1111
enum Segment
{
	s0 = 1<<0,
	s1 = 1<<1,
	s2 = 1<<2,
	s3 = 1<<3,
	s4 = 1<<4,
	s5 = 1<<5,
	s6 = 1<<6,
	s7 = 1<<7,
	top = s0|s1|s2|s3,
	bottom = s3|s4|s5|s6,
	left = s1|s4,
	right = s2|s5,

	digit0 = left|right|s0|s6,
	digit1 = right,
	digit2 = s0|s2|s3|s4|s6,
	digit3 = right|s0|s3|s6,
	digit4 = right|s1|s3,
	digit5 = s0|s1|s3|s5|s6,
	digit6 = bottom|s1|s0,
	digit7 = right|s0,
	digit8 = top|bottom,
	digit9 = top|s5|s6,
	digitA = top|s4|s5,
	digitB = left|bottom,
	digitC = left|s0|s6,
	digitD = right|bottom,
	digitE = left|s0|s3|s6,
	digitF = left|s0|s3
};

struct Sample
{
	unsigned char value;
	Segment segment;
};

Sample samples[] =
{
	{ 0x0, digit0 },
	{ 0x1, digit1 },
	{ 0x2, digit2 },
	{ 0x3, digit3 },
	{ 0x4, digit4 },
	{ 0x5, digit5 },
	{ 0x6, digit6 },
	{ 0x7, digit7 },
	{ 0x8, digit8 },
	{ 0x9, digit9 },
	{ 0xA, digitA },
	{ 0xB, digitB },
	{ 0xC, digitC },
	{ 0xD, digitD },
	{ 0xE, digitE },
	{ 0xF, digitF }
};
