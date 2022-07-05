#ifndef COMMAND_H
#define COMMAND_H
#include <stdio.h>
#include <cstring>
#include "list.h"
#include "record.h"
#include "operation.h"
#include "command_type.h"
#define LEN 1234
#define MAX_EL 3
class command: public record
{
private:
	condition c_name = condition::none;
	condition c_phone = condition::none;
	condition c_group = condition::none;
	operation op=operation::none;
	ordering order[3]={ordering::none,ordering::none,ordering::none };
	ordering order_by[3]={ordering::none,ordering::none,ordering::none };
	command_type type=command_type::none;
public:
	bool onlygroup()
	{
		if(c_name==condition::none && c_group==condition::eq && c_phone==condition::none)
			return true;
		return false;
	}
	bool onlyphone()
	{
		if(c_name==condition::none && c_group==condition::none && c_phone==condition::eq)
			return true;
		return false;
	}
	bool name_phone()
	{
		if(c_name==condition::eq && c_group==condition::none && c_phone==condition::eq)
			return true;
		return false;
	}
	bool group_name_phone()
	{
		if(c_name==condition::eq && c_group==condition::eq && c_phone==condition::eq)
			return true;
		return false;
	}
	bool group_name()
	{
		if(c_name==condition::eq && c_group==condition::eq)
			return true;
		return false;
	}
	bool group_phone()
	{
		if(c_phone==condition::eq && c_group==condition::eq)
			return true;
		return false;
	}
	bool nameonly()
	{
		if(c_name==condition::eq && c_group==condition::none && c_phone==condition::none)
			return true;
		return false;
	}
	ordering * get_order()
	{
		return order;
	}
	command_type get_type()
	{
		return type;
	}
	ordering * get_orderby()
	{
		return order_by;
	}
	command () = default;
	~command () = default;
	bool parse (char * string);
	void print (FILE *fp = stdout) const;
	bool apply (const record& x) const;
	bool set_phone_condition(const char *str);
	bool set_name_condition(const char *str);
	bool set_group_condition(const char *str);
	//int applyby(list<record> &x);
};
#endif // COMMAND_H.