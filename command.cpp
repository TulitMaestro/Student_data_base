#include "command.h"
bool command::set_name_condition(const char *str)
{
	if (!strcmp (str, "="))
		 c_name= condition::eq;
	else if (!strcmp (str, "<>")) 
		c_name = condition::ne;
	else if (!strcmp (str, "<"))
		c_name = condition::lt;
	else if (!strcmp (str, ">"))
		c_name = condition::gt;
	else if (!strcmp (str, "<="))
		c_name = condition::le;
	else if (!strcmp (str, ">="))
		c_name = condition::ge;
	else if (!strcmp (str, "like"))
		c_name = condition::like;
	else if (!strcmp (str, "not like"))
		c_name = condition::nlike;
	else
		return false;
	return true;
}

bool command::set_phone_condition(const char *str)
{
	if (!strcmp (str, "="))
		 c_phone= condition::eq;
	else if (!strcmp (str, "<>")) 
		c_phone = condition::ne;
	else if (!strcmp (str, "<"))
		c_phone = condition::lt;
	else if (!strcmp (str, ">"))
		c_phone= condition::gt;
	else if (!strcmp (str, "<="))
		c_phone = condition::le;
	else if (!strcmp (str, ">="))
		c_phone= condition::ge;
	else if (!strcmp (str, "like"))
		c_phone = condition::like;
	else if (!strcmp (str, "not like"))
		c_phone = condition::nlike;
	else
		return false;
	return true;
}

bool command::set_group_condition(const char *str)
{
	if (!strcmp (str, "="))
		 c_group= condition::eq;
	else if (!strcmp (str, "<>")) 
		c_group= condition::ne;
	else if (!strcmp (str, "<"))
		c_group = condition::lt;
	else if (!strcmp (str, ">"))
		c_group= condition::gt;
	else if (!strcmp (str, "<="))
		c_group = condition::le;
	else if (!strcmp (str, ">="))
		c_group= condition::ge;
	else if (!strcmp (str, "like"))
		c_group = condition::like;
	else if (!strcmp (str, "not like"))
		c_group = condition::nlike;
	else
		return false;
	return true;
}

bool command::parse(char *str)
{
	//char *n=nullptr;
	int max_el=3;
	int p=0,g=0;
	bool flag=false;
	int i=0;
	bool orderflag=false;
	int pos=0;
	char namer[LEN];
	c_name = condition::none;
	c_phone = condition::none;
	c_group = condition::none;
	type=command_type::none;
	op=operation::none;
	char tabl1[]=" ,";
	char tabl2[]=" ,()";
	char * tabl=tabl1;
	char *pch=strtok(str,tabl);
	order[0]=ordering::none;
	order[1]=ordering::none;
	order[2]=ordering::none;
	init (nullptr, 0, 0);
	while(pch!=NULL)
	{
		if(!flag)
		{
			if(pch!=NULL && strcmp(pch,"where")==0)
			{
				flag=true;
			}
			else if(pch!=NULL && strcmp(pch,"select")==0)
			{
				type=command_type::select;
			}
			else if(pch!=NULL && strcmp(pch,"delete")==0)
			{
				type=command_type::del;
			}
			else if(pch!=NULL && strcmp(pch,"insert")==0)
			{
				type=command_type::insert;
			}
			else if(pch!=NULL && strcmp(pch,"quit")==0)
			{
				type=command_type::quit;
			}
			else if(pch!=NULL && strstr(pch,"stop"))
			{
				type=command_type::stop;
			}
			switch(type)					// type
			{
				case::command_type::select:
						if(pch!=NULL && strcmp(pch,"name")==0)
						{
							if(pos<3)
							order[pos]=ordering::name;
							pos++;
						}
						else if(pch!=NULL && strcmp(pch,"phone")==0)
						{
							if(pos<3)
							order[pos]=ordering::phone;
							pos++;
						}
						else if(pch!=NULL && strcmp(pch,"group")==0)
						{
							if(pos<3)
								order[pos]=ordering::group;
							pos++;
						}
						else if(pch!=NULL && strcmp(pch,"*")==0)
						{
							order[0]=ordering::name;
							order[1]=ordering::phone;
							order[2]=ordering::group;
						}
					break;
				case::command_type::insert:
					flag=true;
					tabl=tabl2;
					break;
				case:: command_type::del:
					break;
				case::command_type::quit:
					return true;
					break;
				case::command_type::stop:
					return true;
					break;
				case:: command_type::none:
					break;
			}
		}
		else
		{	
			if(type==command_type::insert)
			{
				if(pch!=NULL && sscanf(pch,"%s",namer)!=1) return false;
				pch=strtok(NULL,tabl);
				if(pch!=NULL && sscanf(pch,"%d",&p)!=1)return false;
				pch=strtok(NULL,tabl);
				if(pch!=NULL && sscanf(pch,"%d",&g)!=1)return false;
				break;
			}

			if(type==command_type::select || type==command_type::del)  //select+del
			{	
				if(orderflag)
				{
					if(strcmp(pch,"name")==0 && i<max_el)
					{
						order_by[i]=ordering::name;
						i++;
					}else
					if(strcmp(pch,"phone")==0 && i<max_el)
					{
						order_by[i]=ordering::phone;
						i++;
					}
					else
					if(strcmp(pch,"group")==0 && i<max_el)
					{
						order_by[i]=ordering::group;
						i++;
					}
				}
				else
				if(pch!=NULL && strcmp(pch,"order")==0)   //мегра сортировка вещь хорошая, но работать не хочет
				{
					
					pch=strtok(NULL,tabl);
					if(pch!=NULL && strcmp(pch,"by")==0)
					{
						i=0;
						orderflag=true;
					}
				}
				else
				if(strcmp(pch,"name")==0)
				{
					pch=strtok(NULL,tabl);
					
					if(pch!=NULL && strcmp(pch,"not")==0)
					{
						pch=strtok(NULL,tabl);
						if(pch!=NULL && !strcmp(pch,"like")==0)
							return false;
						if(!set_name_condition("not like"))
							return false;
						pch=strtok(NULL,tabl);
						if(pch!=NULL && sscanf(pch,"%s",namer)!=1)return false;

					}else
					{

						if(!set_name_condition(pch))
							return false;
						pch=strtok(NULL,tabl);
						if(pch!=NULL && sscanf(pch,"%s",namer)!=1)return false;
					}

				}
				else if (strcmp(pch,"phone")==0)
				{
					pch=strtok(NULL,tabl);
					if(pch!=NULL && strcmp(pch,"not")==0 )
					{
						return false;
					}else
					{
						if(!set_phone_condition(pch))
							return false;
						pch=strtok(NULL,tabl);
						if(pch!=NULL && sscanf(pch,"%d",&p)!=1)
							return false;

					}
				}else if(strcmp(pch,"group")==0)
				{
					pch=strtok(NULL,tabl);
					if(pch!=NULL && strcmp(pch,"not")==0)
					{
						return false;
					}else
					{
						if(!set_group_condition(pch))
							return false;
						pch=strtok(NULL,tabl);
						if( pch!=NULL && sscanf(pch,"%d",&g)!=1)
							return false;

					}
				}
				else if(pch!=NULL && strcmp(pch,"and")==0)
				{	if(op==operation::lor)
						return false;
					op=operation::land;
				}
				else if( pch!=NULL && strcmp(pch,"or")==0)
				{
					if(op==operation::land)
						return false;
					op=operation::lor;
				}
			}

		}
		pch=strtok(NULL,tabl);

	}
	if(init(namer,p,g))return false;
	return true;
}
void command::print(FILE *fp)const
{
	fprintf(fp,"conditions: %d %d %d %d\n", (int)c_name, (int)c_phone, (int)c_group, (int)type);
}

bool command:: apply(const record& x)const
{
	if(type==command_type::select || type==command_type::del)
	{
		if(op==operation::land || op==operation::none)
		{
			if(compare_name(c_name,x) && compare_phone(c_phone,x) && compare_group(c_group,x)) return true;
		}
		if(op==operation::lor)
		{	
			bool flag1=false;
			bool flag2=false;
			bool flag3=false;
			if(c_name==condition::none) flag1=false;
			else flag1=compare_name(c_name,x);

			if(c_phone==condition::none) flag2=false;
			else flag2=compare_phone(c_phone,x);

			if(c_group==condition::none) flag3=false;
			else flag3=compare_group(c_group,x);

			if(flag1 || flag2 || flag3) return true;
		}
	}
	return false;
}

/*static int compare(record_address &x1, record_address &x2,ordering *order_by)
{
	int i=0;
	int res=0;
	ordering *order= (ordering*) order_by;

	for(i=0;i<MAX_EL;i++)
	{
		switch(order[i])
		{
			case ordering::name:
				if((*x1).get_name()!=nullptr && (*x2).get_name()!=nullptr)
				res=strcmp((*x1).get_name(),(*x2).get_name());
				if(res!=0)
					return res;
				break;
			case ordering::phone:
				res=(*x1).get_phone()-(*x2).get_phone();
				if(res!=0)
					return res;
				break;
			case ordering::group:
				res=(*x1).get_phone()-(*x2).get_phone();
				if(res!=0)
					return res;
				break;
			case ordering::none:
				break;
		}
	}
	return res;
}*/