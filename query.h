#ifndef QUERY
#include <cstdio>
#include <cstring>

enum class states {
	start,
	check_next,
	literal_symbol,
	underline,
	backslash,
	percent,
	range,
	endfun,
};

class query {
private:
	bool mis = false;
	const char * str1 = nullptr;
	const char * str2 = nullptr;
	states state = states::start;
	char left = 0;
	char right = -1;
	int shift = 0;
	const char * regexp_checkpoint = nullptr;
	const char * str_checkpoint = nullptr;
	int skip = -1;
	
	
	query () = default;
	query (const char * str, const char * regexp) : query () {
		str1 = str, str2 = regexp;
		state = states::start;
	}
	~query() =default;
	
	int range_processing ()
	{
		char t;
		bool reversing=false;
		shift = 0;
		mis = true;
		str2++;
		shift++;
		if (*str2 == '^')
		{
			reversing = true;
			str2++, shift++;
		}
		
		if (*str2 == '\\')
			str2++, shift++;
		left = *str2;
		
		str2++, shift++;
		if (*str2 != '-')
		{
			str2 -= shift;
			shift = 1;
			return 1;
		}
		
		str2++, shift++;
		if (*str2 == '\\')
			str2++, shift++;
		right = *str2;
		
		str2++, shift++;
		if (*str2 != ']')
		{
			str2 -= shift;
			shift = 1;
			return 1;
		}
		
		shift = 0;
		t=*str1;
		str2++;
		if (reversing)
		{
			if ((t < left) || (t > right))
				return 1;
			return 0;
		}
		else
		{
			if ((t >= left) && (t <= right))
				return 1;
			return 0;
		}
	}
	
	int next () {
		switch (state) {
			case states::start:
			case states::check_next:
				if (shift > 0)
				{
					shift--;
					state = states::literal_symbol;
					break;
				}
				
				if (*str2 == '\\') {
					state = states::backslash;
				} else if (*str2 == '%') {
					state = states::percent;
					break;
				} else if (*str2 == '_') {
					state = states::underline;
				} else if (*str2 == '[') {
					if (symbolcheck(str2))
						state = states::range;
					else
						state = states::literal_symbol;
				} else if (*str2 == '\0') {
					state = states::endfun;
				} else state = states::literal_symbol;
				
				if (*str1 == '\0')
					state = states::endfun;
				break;
				
			case states::literal_symbol:
				if (*str1 != *str2)
					state = states::endfun;
				else
				{
					str1++, str2++;
					state = states::check_next;
				}
				break;
			
			case states::backslash:
				str2++;
				state = states::literal_symbol;
				break;
			
			case states::underline:
				if (*str1 == '\0')
				{
					state = states::endfun;
				}
				else
				{
					str2++, str1++;
					state = states::check_next;
				}
				break;
				
			case states::percent:
				if (*str2 == '%')
				{
					if (regexp_checkpoint == nullptr)
					{
						str2 += strspn (str2, "%"); 
						regexp_checkpoint = str2;
						str_checkpoint = str1;
						state = states::check_next;
					}
					else
					{
						return checker (str1, str2);
					}
				}
				break;
	
			case states::range:
				if (!range_processing ())
				{
					mis = true;
					state = states::endfun;
				}
				else
				{
					str1++;
					state = states::check_next;
				}
				break;
			case states::endfun:
				if ((*str2 == '\0') && (*str1 == '\0'))
					return 1;
				if (regexp_checkpoint != nullptr)
				{
					skip++;
					str2 = regexp_checkpoint;
					str1 = str_checkpoint + skip;
					state = states::check_next;
					if (*str1 != '\0')
						break;
				}
				if (mis)
					return 0;
				if (*str1 != *str2)
					return 0;
				break;
			default:
				break;
		}
		return -1;
	}


	int symbolcheck(const char * str)
	{
		if (!(strlen (str) >= 5))
			return 0;
		if ((strlen (str) - strcspn (str, "]")) <= 0)
			return 0;
		return 1;
	}

public:
	static int checker (const char * str, const char * regexp)
	{
		query checker (str, regexp);
		int result = 0;
		int i;
		
		for (i = 0; ((result = checker.next()) != 1) && (result != 0) && (i < 1e6); i++){}
		return result;
	}
};
#endif // query.