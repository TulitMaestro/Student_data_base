#ifndef condition_H
#define condition_H
enum class condition
{
	none,
	eq, // equal
	ne, //not equal
	lt, // less than <
	gt, //great than >
	le, // <=
	ge, // >=
	like, //strings only: match pattern
	nlike,//strings only: not match pattern
};
#endif // condition_H