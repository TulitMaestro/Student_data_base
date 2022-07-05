#ifndef DATABASE_H
#define DATABASE_H
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
#include "stdio.h"
#include "rb_tree.h"
#include "record.h"
#include "command.h"
#include "b_tree.h"
#define GROUP_LEN 1000
class database
{
private:
	list<record> * listoc=nullptr;
	rb_tree<record_ptr> **rb_hash=nullptr;
	b_tree<record_ptr> **b_hash=nullptr;
	
	rb_tree<record_ptr> ***group_rb_hash=nullptr;
	b_tree<record_ptr> ***group_b_hash=nullptr;
	
	int k1=0;
	int k2=0;
	int rb_k=1;
	int b_k=1;
	int m_forb=2;

	int group_hash=GROUP_LEN;
	int func_hash_phone(int phone,int k)
	{
		int res=0;
		int i=0;
		int t=0;
		int n=0;
		t=phone;
		for(;t!=0;t/=10)
			n++;
		n=n-k1;
		if(n<0)n=0;
		t=phone;
		for(i=0;i<n;i++)
			t/=10;
		res=t % k;
		return res;

	}
	int func_hash_name (const char * name, int k)
	{
		int res = 0;
		int t=0;

		if (name == nullptr)
			return res;
		t=1;
		for(int i=0; i<k2;i++)
		{
			
			if(!(name[i]))
			{
				for(;i<k2;i++)
					res*=10;
				break;
			}
			else
			{
				res+=((int) name[i]%10)*t;
			}
			t*=10;
		}
		res=res%k;
		return res;
	}
	int func_hash_group(int group)
	{
		int res=0;
		res=group%group_hash;
		return res;
	}
public:
	void * rb_fun_cmp_arg=nullptr;
	database()=default;
	~database()
	{
		delete_all();
	}
	database(int k10,int k20,int m):database()
	{
		this->m_forb=m;
		k1=k10;
		rb_k=1;
		for(int i=0;i<k10;i++)
		{
			rb_k*=10;
		}

		k2=k20;
		b_k=1;
		for(int i=0;i<k2;i++)
		{
			b_k*=10;
		}
	}
	void delete_all()
	{
		int a=0;
		if(listoc!=nullptr)
		{
			delete listoc;
			listoc=nullptr;
		}
		if(rb_hash!=nullptr)
		{
			for(int i=0;i<rb_k;i++)
			{
				if(rb_hash[i]!=nullptr)
					delete rb_hash[i];
				rb_hash[i]=nullptr;
			}
			delete[] rb_hash;
			rb_hash=nullptr;
		}
		if(b_hash!=nullptr)
		{
			for(int i=0;i<b_k;i++)
			{
				if(b_hash[i]!=nullptr)
					delete b_hash[i];
				b_hash[i]=nullptr;
			}
			delete[] b_hash;
			b_hash=nullptr;
		}
		if(group_rb_hash!=nullptr) //удаления хэшгруппы
		{
			for(int i=0;i<group_hash;i++)
			{
				if(group_rb_hash[i]!=nullptr)
				{
					for(a=0;a<rb_k;a++)
					{
						if(group_rb_hash[i][a]!=nullptr)
							delete group_rb_hash[i][a];
						group_rb_hash[i][a]=nullptr;
					}
					delete[] group_rb_hash[i];
					group_rb_hash[i]=nullptr;
				}
			}
			delete[] group_rb_hash;
			group_rb_hash=nullptr;
		}
		if(group_b_hash!=nullptr) //удаления хэшгруппы
		{
			for(int i=0;i<group_hash;i++)
			{
				if(group_b_hash[i]!=nullptr)
				{
					for(a=0;a<b_k;a++)
					{
						if(group_b_hash[i][a]!=nullptr)
							delete group_b_hash[i][a];
						group_b_hash[i][a]=nullptr;
					}
					delete[] group_b_hash[i];
					group_b_hash[i]=nullptr;
				}
			}
			delete[] group_b_hash;
			group_b_hash=nullptr;
		}
	}

	static int rb_tree_cmp (record_ptr& p1, record_ptr& p2)
	{
		if(p1.ptr==nullptr && p2.ptr==nullptr)
			return 0;
		if(p1.ptr==nullptr)
			return -1;
		if(p2.ptr==nullptr)
			return 1;
		return p1.ptr->get_phone() - p2.ptr->get_phone();
	}
	
	static int b_tree_cmp (record_ptr& p1, record_ptr& p2)
	{
		if (!(p1.ptr != nullptr && p2.ptr != nullptr))
			return 0;
		if (!(p1.ptr->get_name () != nullptr && p2.ptr->get_name () != nullptr))
			return 0;
		return strcmp (p1.ptr->get_name (), p2.ptr->get_name ());
	}

	static int b_tree_cmp_forfind(record*& p1, command & pain)
	{	
		if(pain.onlygroup())
		{
			if(p1!=nullptr && pain.get_group()==p1->get_group())
				return 0;
			return 1;
		}
		else
		if(pain.group_name_phone())
		{	
			if (!(p1!= nullptr && pain.get_name()!= nullptr) || (p1->get_phone()!=pain.get_phone()))
				return -1;
			if (!(p1->get_name () != nullptr && pain.get_name()!= nullptr) || (p1->get_phone()!=pain.get_phone()) )
				return 1;
			if(p1!=nullptr && pain.get_group()==p1->get_group())
				return strcmp (p1->get_name (), pain.get_name());
		}
		if(pain.name_phone())
		{
			if (!(p1!= nullptr && pain.get_name()!= nullptr) || (p1->get_phone()!=pain.get_phone()))
				return -1;
			if (!(p1->get_name () != nullptr && pain.get_name()!= nullptr) || (p1->get_phone()!=pain.get_phone()) )
				return 1;
			return strcmp (p1->get_name (), pain.get_name());
		}
		else
		{
			if (!(p1!= nullptr && pain.get_name()!= nullptr))
				return -1;
			if (!(p1->get_name () != nullptr && pain.get_name()!= nullptr))
				return 1;
			return strcmp (p1->get_name (), pain.get_name());
		}
		return 1;
	}

	int read(FILE *fp)
	{
		int ret=0,index;
		int group=0;
		rb_tree_node<record_ptr> *t=nullptr;
		record_ptr tb;
		listoc=new list<record>;
		
		if(listoc==nullptr)
		{	
			delete_all();
			return 1;
		}
		
		ret=listoc->read(fp);
		if(ret!=0)
		{
			delete_all();
			return 2;
		}

		rb_hash= new rb_tree<record_ptr>* [rb_k]; //rb_создаем

		if(rb_hash==nullptr)
		{
			delete_all();
			return 3;
		}

		for(int i=0;i<rb_k;i++)
			rb_hash[i]=nullptr;

		for(int i=0;i<rb_k;i++)
		{
			rb_hash[i]=new rb_tree<record_ptr> (rb_tree_cmp);
			if(rb_hash[i]==nullptr)
			{
				delete_all();
				return 4;
			}
		}
		for(list_node<record> * cur=listoc->get_head();cur!=nullptr; cur=cur->get_next()) //рб читаем
		{	

			index=func_hash_phone(cur->get_phone(),rb_k);
			if(index>=rb_k)
			{
				printf("ERROR_read rb_tree\n");
				delete_all();
				return 6;
			}
			t=new rb_tree_node<record_ptr>;
			if(t==nullptr)
			{
				delete_all();
				return 5;
			}
			t->init_ptr((record*)cur);
			rb_hash[index]->add_node(rb_hash[index]->root,t,nullptr);
			t=nullptr;
		}

		b_hash=new b_tree<record_ptr>* [b_k];

		if(b_hash==nullptr)
		{
			delete_all();
			return 6;
		}

		for(int i=0;i<b_k;i++)
			b_hash[i]=nullptr;

		for(int i=0;i<b_k;i++)
		{
			b_hash[i]=new b_tree<record_ptr> (b_tree_cmp,m_forb);
			if(b_hash[i]==nullptr)
			{
				delete_all();
				return 7;
			}
		}
		for(list_node<record> * cur=listoc->get_head();cur!=nullptr; cur=cur->get_next()) //б читаем
		{	

			index=func_hash_name(cur->get_name(),b_k);
			if(index>=b_k)
			{
				printf("ERROR_read b_tree\n");
				delete_all();
				return 8;
			}
			tb.init_ptr((record*)cur);
			b_hash[index]->add_node(b_hash[index]->root,tb);
		}

		group_rb_hash= new rb_tree<record_ptr> **[group_hash]; // group rb читаем
		
		if(group_rb_hash==nullptr)
		{
			delete_all();
			return 9;
		}

		for(int i=0;i<group_hash;i++)
			group_rb_hash[i]=nullptr;

		for(int i=0;i<group_hash;i++)
		{
			group_rb_hash[i]=new rb_tree<record_ptr>* [rb_k];
			if(group_rb_hash[i]==nullptr)
			{
				delete_all();
				return 10;
			}

			for(int a=0;a<rb_k;a++)
				group_rb_hash[i][a]=nullptr;

			for(int a=0;a<rb_k;a++)
			{
				group_rb_hash[i][a]=new rb_tree<record_ptr> (rb_tree_cmp);
				if(group_rb_hash[i][a]==nullptr)
				{
					delete_all();
					return 11;
				}
			}
		}

		for(list_node<record> * cur=listoc->get_head();cur!=nullptr; cur=cur->get_next()) 
		{	
			group=func_hash_group(cur->get_group());
			index=func_hash_phone(cur->get_phone(),rb_k);
			if(index>=rb_k)
			{
				printf("ERROR_read rb_tree\n");
				delete_all();
				return 12;
			}
			t=new rb_tree_node<record_ptr>;
			if(t==nullptr)
			{
				delete_all();
				return 13;
			}
			t->init_ptr((record*)cur);
			group_rb_hash[group][index]->add_node(group_rb_hash[group][index]->root,t,nullptr);
			t=nullptr;
		}

		group_b_hash=new b_tree<record_ptr> ** [group_hash]; // group_b читаем

		if(group_b_hash==nullptr)
		{
			delete_all();
			return 14;
		}
		for(int i=0;i<group_hash;i++)
			group_b_hash[i]=nullptr;

		for(int i=0;i<group_hash;i++)
		{
			group_b_hash[i]=new b_tree<record_ptr>* [b_k];
			if(group_b_hash[i]==nullptr)
			{
				delete_all();
				return 15;
			}

			for(int a=0;a<b_k;a++)
				group_b_hash[i][a]=nullptr;

			for(int a=0;a<b_k;a++)
			{
				group_b_hash[i][a]=new b_tree<record_ptr> (b_tree_cmp,m_forb);
				if(group_b_hash[i][a]==nullptr)
				{
					delete_all();
					return 16;
				}
			}
		}

		for(list_node<record> * cur=listoc->get_head();cur!=nullptr; cur=cur->get_next()) //б читаем
		{	
			group=func_hash_group(cur->get_group());
			index=func_hash_name(cur->get_name(),b_k);
			if(index>=b_k)
			{
				printf("ERROR_read b_tree\n");
				delete_all();
				return 17;
			}
			tb.init_ptr((record*)cur);
			group_b_hash[group][index]->add_node(group_b_hash[group][index]->root,tb);
		}

		return 0;
	}

	int fun(char *buf,int fd) // основа основ, корень корней, древнее зло всех утечек
	{
		command pain;
		bool ret=false;
		int group;
		int res = 0;
		int key=0;
		char *t=nullptr;
		record_ptr tb;
		record_ptr tb1;
		list_node<record> *cur=nullptr;
		list_node<record> *next=nullptr;

		list_node<record> *t1=nullptr;
		rb_tree_node<record_ptr> *buf1=nullptr;


		rb_tree_node<record_ptr> *fordel=nullptr;

		t=strchr(buf,';');
		if(t!=nullptr)
			*t='\0';
		ret = pain.parse (buf);
		if (ret)
		{
			if(pain.get_type()==command_type::select)
			{	

				if(pain.onlygroup())
				{
					group=func_hash_group(pain.get_group());
					for (key = 0; key<b_k; key++)
					{
						if(group_b_hash[group][key]!=nullptr)
							findall_btree(group_b_hash[group][key],group_b_hash[group][key]->root,pain,res,fd);
					}
					
				}
				else
				if(pain.name_phone())
				{
					key=func_hash_name(pain.get_name(),b_k);
					if(b_hash[key]!=nullptr)
						findall_btree(b_hash[key],b_hash[key]->root,pain,res,fd);

				}
				else
				if(pain.group_name_phone())
				{
					group=func_hash_group(pain.get_group());
					key=func_hash_name(pain.get_name(),b_k);
					if(group_b_hash[group][key]!=nullptr)
						findall_btree(group_b_hash[group][key],group_b_hash[group][key]->root,pain,res,fd);
				}
				else
				if(pain.group_name())
				{
					group=func_hash_group(pain.get_group());
					key=func_hash_name(pain.get_name(),b_k);
					if(group_b_hash[group][key]!=nullptr)
						findall_btree(group_b_hash[group][key],group_b_hash[group][key]->root,pain,res,fd);
				}
				else
				if(pain.group_phone())
				{
					group=func_hash_group(pain.get_group());
					key=func_hash_phone(pain.get_phone(),rb_k);
					if(group_rb_hash[group][key]!=nullptr)
						findall(pain.get_phone(),group_rb_hash[group][key],group_rb_hash[group][key]->get_root(),res,pain,fd);
				}
				else
				if(pain.onlyphone())
				{	
					key=func_hash_phone(pain.get_phone(),rb_k);
					if(rb_hash[key]!=nullptr)
						findall(pain.get_phone(),rb_hash[key],rb_hash[key]->get_root(),res,pain,fd);
				}
				else
				if(pain.nameonly())
				{
					key=func_hash_name(pain.get_name(),b_k);
					if(b_hash[key]!=nullptr)
						findall_btree(b_hash[key],b_hash[key]->root,pain,res,fd);
				}	
				else
				{
					for (cur = listoc->get_head(); cur != nullptr; cur = cur->get_next())
					{
						if (pain.apply (*cur))
						{
							cur->print (fd,pain.get_order());
							res++;
						}
					}
				}

			}
			if(pain.get_type()==command_type::insert)
			{		
					
				t1=nullptr;	
				t1=listoc->insert_ptr( *pain); // вставка в листок
				group=func_hash_group(pain.get_group());
				key=func_hash_phone(pain.get_phone(),rb_k);
				if(t1!=nullptr) //вставка в rb
				{
					buf1=new rb_tree_node<record_ptr>;
					buf1->init_ptr((record *)t1);
					rb_hash[key]->add_node(rb_hash[key]->root,buf1,nullptr);

					buf1=new rb_tree_node<record_ptr>;
					buf1->init_ptr((record *)t1);
					group_rb_hash[group][key]->add_node(group_rb_hash[group][key]->root,buf1,nullptr);
					key=func_hash_name(t1->get_name(),b_k);
					tb.init_ptr((record *)t1);
					b_hash[key]->add_node(b_hash[key]->root,tb);// вставка в б
					tb1.init_ptr((record *)t1);
					group_b_hash[group][key]->add_node(group_b_hash[group][key]->root,tb1);
				}

			}
			if(pain.get_type()==command_type::del)  // не повезло не повезло
			{
				for(cur=listoc->get_head();cur!=nullptr;)
				{
					next=cur->get_next();
					if (pain.apply (*cur))
					{
						group=func_hash_group(cur->get_group());
						key=func_hash_phone(cur->get_phone(),rb_k);
						fordel=find_ptr(rb_hash[key]->get_root(),cur);
						if(fordel!=nullptr)
						{
							fordel->init_ptr(nullptr);
						}
						
						fordel=find_ptr(group_rb_hash[group][key]->get_root(),cur);
						if(fordel!=nullptr)
						{
							fordel->init_ptr(nullptr);
						}
						
						key=func_hash_name(cur->get_name(),b_k);
						nullptr_bptr(b_hash[key],b_hash[key]->root,cur);
						nullptr_bptr(group_b_hash[group][key],group_b_hash[group][key]->root,cur);
						listoc->del(cur);
					}
					cur=next;
				}
			
			}
			if(pain.get_type()==command_type::quit)
			{
				codes_client_server err_code = codes_client_server::QUIT_OR_STOP;
			    if (write (fd, &err_code, sizeof (codes_client_server)) != (int) sizeof (codes_client_server))
			    {
			        perror ("write length");
			        exit (EXIT_FAILURE);
			    }
			    return -1;
			}
			if(pain.get_type()==command_type::stop)
			{
				codes_client_server err_code = codes_client_server::QUIT_OR_STOP;
			    if (write (fd, &err_code, sizeof (codes_client_server)) != (int) sizeof (codes_client_server))
			    {
			        perror ("write length");
			        exit (EXIT_FAILURE);
			    }
			    return -2;
			}
		}
		codes_client_server err_code = codes_client_server::OUTPUT_END;
	    if (write (fd, &err_code, sizeof (codes_client_server)) != (int) sizeof (codes_client_server))
	    {
	        perror ("write length");
	        exit (EXIT_FAILURE);
	    }

		return res;
	}
	void findall(int key,rb_tree<record_ptr> *mytree,rb_tree_node<record_ptr> *startpos,int& res,command& pain,int fd)
	{
		rb_tree_node<record_ptr> *rb_nooder=nullptr;
		record* t1=nullptr;
		rb_nooder=mytree->find(startpos,pain.get_phone());
		if(rb_nooder==nullptr)
		{
			return;
		}
		if(rb_nooder->get_ptr()!=nullptr)
			t1=rb_nooder->ptr;
		if(t1!=nullptr)
		{
			t1->print(fd,pain.get_order());
			res++;
		}
		if(rb_nooder->get_right()!=nullptr)
			findall(key,mytree,rb_nooder->get_right(),res,pain,fd);
		if(rb_nooder->get_left()!=nullptr)
			findall(key,mytree,rb_nooder->get_left(),res,pain,fd);
		
	}

	rb_tree_node<record_ptr>* find_ptr(rb_tree_node<record_ptr> *start,record * ptr)
	{
		rb_tree_node<record_ptr>* t=start;
		rb_tree_node<record_ptr>* ret=nullptr;
		if(start==nullptr)
			return nullptr;
		if((t->get_ptr())==ptr)
		{
			ret=t;
			return ret;
		}
		if(t->get_right()!=nullptr)
			ret=find_ptr(t->get_right(),ptr);
		if(ret!=nullptr)
			return ret;
		if(t->get_left()!=nullptr)
			ret=find_ptr(t->get_left(),ptr);
		return ret;
	}
	
	void findall_btree(b_tree<record_ptr> *mytree,b_tree_node<record_ptr> *x,command& pain,int &res,int fd)
	{
		int m=mytree->get_m();
		int n=0;
		if(x==nullptr)
			return;
		n=x->size;
		for(int i=0;i<n;i++)
		{
			if( x->values[i].get_ptr()!=nullptr && b_tree_cmp_forfind(x->values[i].ptr,pain)==0)
			{
				x->values[i].ptr->print(fd,pain.get_order());
				res++;
			}
			
		}

		if(x->children==nullptr)
			return;
		for(int i=0;i<=2*m && x->children[i]!=nullptr;i++)
		{
			findall_btree(mytree,x->children[i],pain,res,fd);
			
		}

		return;	
	}

	void nullptr_bptr(b_tree<record_ptr> *mytree,b_tree_node<record_ptr> *x,record * ptr)
	{
		int m=mytree->get_m();
		int n=x->size;
		if(x==nullptr)
			return;
		for(int i=0;i<n;i++)
		{
			if( x->values[i].get_ptr()!=nullptr && x->values[i].get_ptr()==ptr)
			{
				x->values[i].init_ptr(nullptr);
				return;
			}
			
		}

		if(x->children==nullptr)
			return;
		for(int i=0;i<=2*m && x->children[i]!=nullptr;i++)
		{
			nullptr_bptr(mytree,x->children[i],ptr);
			
		}

		return;	
	}


};
#endif //DATABASE_H