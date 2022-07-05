#ifndef list_H
#define list_H
#include <stdio.h>
#include "record.h"
#include "rb_tree.h"
#include "ordering.h"
template <class T> class list;
template <class T>
class list_node: public T
{
private:
	list_node* next=nullptr;
	list_node* prev=nullptr;
public:
	list_node()=default;
	~list_node()=default;
	T* get_t()
	{
		return (T*) this;
	}
	void deleter()
	{
		delete this;
	}
	list_node(const list_node& x):T(x)
	{
		next=nullptr;
		prev=nullptr;
	}
	
	list_node(list_node&& x):T((T&&)x)
	{
		next=x.next;
		prev=x.prev;
		x.prev=nullptr;
		x.next=nullptr;
	}
	list_node* get_next()
	{
		return next;
	}
	list_node* get_prev()
	{
		return prev;
	}
	
	friend class list<T>;
	friend class rb_tree_node<T>;
};

template <class T>
class list
{
private:
	list_node<T>* head=nullptr;
public:
	list()=default;
	~list()
	{
		delete_list();
	}

	list(list&& x)
	{
		head=x.head;
		x.head=nullptr;
	}
	list_node<T>* get_head()const
	{
		return head;
	}
	int get_length()
	{
		list_node<T> *curr=head;
		int i=0;
		while(curr!=nullptr)
		{
			curr=curr->next;
			i++;
		}
		return i;
	}

	int read(FILE *fp=stdin)
	{
		list_node<T> *last=nullptr;
		list_node<T> *t = nullptr;
		int ret;
		
		if (head != nullptr)
			return -1;
		head=new list_node<T>;
		ret=head->read(fp);
		if (ret!= 0)
		{
			delete head;
			head=nullptr;
			return -3;
		}
		for (last = head; ; last = last->next)
		{
			t=new list_node<T>;
			ret = t->read(fp);
			if (ret != 0)
			{
				delete t;
				if (feof (fp))
					break;
				delete_list ();
				return -4;
			} 
			last->next = t;
			t->prev = last;
		}
		if ((!feof (fp)) )
		{
			delete_list ();
			return -5;
		}
		return 0;
	}

	void print(FILE *fp=stdout)
	{
		list_node<T> * cur = head;
		int i=0;
		for (i = 0; (cur != nullptr); cur = cur->get_next(), i++)
		{
			cur->print (fp);
		}
	}

	void insert(T& x)
	{
		list_node<T> *curr=nullptr;
		list_node<T> *last=nullptr;
		if(head==nullptr)
		{
			head=new list_node<T> ((list_node<T> && )x);
			return ;
		}
		for(curr=head;curr!=nullptr;curr=curr->next)
		{
			if(((T& )x)==((T&)*curr))
				return;
			last=curr;
		}
		curr=new list_node<T> ((list_node<T> &&)x);
		last->next=curr;
		curr->prev=last;
		return;
	}
	list_node<T>* insert_ptr(T& x)
	{
		list_node<T> *curr=nullptr;
		list_node<T> *last=nullptr;
		if(head==nullptr)
		{
			head=new list_node<T> ((list_node<T> && )x);
			return nullptr;
		}
		for(curr=head;curr!=nullptr;curr=curr->next)
		{
			if(((T& )x)==((T&)*curr))
				return nullptr;
			last=curr;
		}
		curr=new list_node<T> ((list_node<T> &&)x);
		last->next=curr;
		curr->prev=last;
		return curr;
	}

	void del(list_node<T>* x)
	{
		if(x==nullptr)
			return;
		if(x->prev!=nullptr)
			x->prev->next=x->next;
		if(x->next!=nullptr)
			x->next->prev=x->prev;
		if(x==head)
			head=head->next;
		x->deleter();

	}

	void delete_list()
	{
		list_node<T> *curr=nullptr;
		while(head !=nullptr)
		{
			curr=head->get_next();
			delete head;
			head=curr;
		}
		head=nullptr;
	}
};
#endif //list_H