#include <string.h>
#include <stdio.h>
#include "record.h"
#include "query.h"

#define LEN 1234

using namespace std;

void writeToClient (int fd, char *buf, int _len)
{
    int nbytes;
    if (_len < 0)
        _len = strlen (buf);
    unsigned len = (unsigned)_len;
    codes_client_server err_code = codes_client_server::SUCCESS;
    if (write (fd, &err_code, sizeof (codes_client_server)) != (int) sizeof (codes_client_server))
    {
        perror ("SERVER:write_length2");
        exit (EXIT_FAILURE);
    }

    const unsigned mask = (1 << 8) - 1;
    char ch;
    for (long unsigned byte = 0; byte < sizeof(unsigned); byte++)
    {
        ch = (char)( (len >> (8*byte)) & mask);
        if (write (fd, &ch, sizeof (char)) != (int) sizeof (char))
        {
            perror ("SERVER:write_length2");
            exit (EXIT_FAILURE);
        }
    }

    for (int i = 0; len > 0; i += nbytes, len -= nbytes)
    {
        nbytes = write (fd, buf + i, len);
        if (nbytes < 0)
        {
            perror ("SERVER:write");
            exit (EXIT_FAILURE);
        }
        else if (nbytes == 0)
        {
            perror ("SERVER:write_truncated");
            exit (EXIT_FAILURE);
        }
    }
}
int record::init (const char *n, int p, int g)
{
	phone = p;
	group = g;
	if (n)
	{
		name = std::make_unique<char []> (strlen (n) + 1);
		if (!name) return -1;
		strcpy (name.get(), n);
	}
	else
		name = nullptr;
	return 0;
}

void record::print (int fd,const ordering order[])
{
	const int max_items = 3;
	const ordering default_ordering[max_items] = {ordering::name, ordering::phone, ordering::group};
	const ordering * p = (order ? order : default_ordering);
	char buf[LEN];
	int len =0;

	for (int i = 0; i < max_items; i++)
		switch (p[i])
		{
		case ordering::name:
			len += sprintf (buf + len, "%s", get_name ());
			break;
		case ordering::phone:
			len += sprintf (buf + len, " %d", get_phone ()); 
			break;
		case ordering::group:
			len += sprintf (buf + len, " %d", get_group ()); 
			break;
		case ordering::none:
			continue;
		}
	len++;
	buf[len]='\0';
	printf("send to client:%d %s\n",len,buf);
    writeToClient (fd, buf, len);
}
int record::read (FILE *fp)
{
	char buf[LEN];
	name = nullptr;
	if (fscanf (fp, "%s%d%d", buf, &phone, &group) != 3)
	{
		if (feof(fp)) return -1;
		return -2;
	}
	if (init (buf, phone, group))
		return -3;
	return 0;
}

bool record::compare_phone (condition x, const record& y) const
{
	switch (x)
	{
		case condition::none: 
			return true;
		case condition::eq: 
			return y.phone == phone;
		case condition::ne: 
			return y.phone != phone;
		case condition::lt:
			return y.phone < phone;
		case condition::gt: 
			return y.phone > phone;
		case condition::le: 
			return y.phone <= phone;
		case condition::ge: 
			return y.phone >= phone;
		case condition::like:
			return false;
		case condition::nlike:
			return false;
	}
	return false;
}

bool record::compare_group (condition x, const record& y) const
{
	switch (x)
	{
		case condition::none: 
			return true; 
		case condition::eq: 
			return y.group == group;
		case condition::ne: 
			return y.group != group;
		case condition::lt: 
			return y.group < group;
		case condition::gt: 
			return y.group > group;
		case condition::le: 
			return y.group <= group;
		case condition::ge:
			return y.group >= group;
		case condition::like: 
			return false;
		case condition::nlike:
			return false;
	}
	return false;
}

bool record::compare_name (condition x, const record& y) const
{
	int res = 0;
	if (get_name() != nullptr) res = strcmp (y.get_name(), this->get_name());
	switch (x)
	{
		case condition::none: 
			return true; 
		case condition::eq: 
			return res == 0;
		case condition::ne: 
			return res != 0;
		case condition::lt: 
			return res < 0;
		case condition::gt: 
			return res > 0;
		case condition::le: 
			return res <= 0;
		case condition::ge: 
			return res >= 0;
		case condition::like: 
			return query::checker(y.get_name(), this->get_name());
		case condition::nlike: 
			return !(query::checker(y.get_name(), this->get_name())); 

	}
	return false;
}
bool record::operator==(const record& x)
{
	if(phone==x.phone || phone==0 || x.phone==0)
	{
		if(group==x.group || group==0 || x.group==0)
		{
			if(name!=nullptr && x.name!=nullptr)
			{
				if(strcmp(get_name(),x.get_name())==0)
					return true;
			}
			else if(name==nullptr && x.name==nullptr)
				return true;
		}
	}
	return false;
}