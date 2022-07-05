#ifndef record_H
#define record_H
#include <memory>
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ordering.h"
#include "condition.h"

class record
{
private:
	std::unique_ptr<char []> name=nullptr;
	int phone=0;
	int group=0;
public:
	record()=default;
	~record()=default;
	char *get_name()const
	{
		return name.get();
	}
	int get_phone()const
	{
		return phone;
	}
	int get_group()const
	{
		return group;
	}
	int init(const char *n,int p,int g);

	record (record &&x)=default;
	record& operator=(record&& x)=default;
	record (const record &x);
	record& operator=(const record&)=delete;
	record& operator*()
	{
		return *this;
	}
	bool operator==(const record& x);
	bool compare_name(condition x, const record& y) const;
	bool compare_phone(condition x,const record& y) const;
	bool compare_group(condition x, const record& y) const;
	void print (int fd,const ordering order[]=nullptr);
	int read (FILE *fp=stdin);
};

class record_ptr
{
private:
	
public:
	record *ptr=nullptr;
	record_ptr()=default;
	void init_ptr(record *adr)
	{
		ptr=adr;
	}
	~record_ptr()=default;
	record* get_ptr()
	{
		return ptr;
	}
	int get_phone()
	{
		if(ptr!=nullptr)
			return ptr->get_phone();
		return 0;
	}
	void print(int fd)
	{
		if(ptr!=nullptr)
			ptr->print(fd);
	}
	int operator== (const record_ptr& x)
	{
		return (*ptr == *(x.ptr));
	}
};
#endif // record_H