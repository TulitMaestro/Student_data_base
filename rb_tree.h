#ifndef RB_TREE
#define RB_TREE
#include <stdio.h>
enum COLORS
	{
			INVALID,
			RED,
			BLACK,
	};
template <class T> class rb_tree;
template <class T> 
class rb_tree_node: public T
{
	
	private:
		rb_tree_node* left=nullptr;
		rb_tree_node* right=nullptr;
		rb_tree_node* parent=nullptr;
		int (*compare_func) (T&, T&) = nullptr;
		COLORS color=INVALID;
	public:
		rb_tree_node()=default;
		rb_tree_node* get_right()
		{
			return right;
		}

		rb_tree_node* get_left()
		{
			return left;
		}
		rb_tree_node* get_parent()
		{
			return parent;
		}
		~rb_tree_node()
		{
			left=nullptr;
			right=nullptr;
			parent=nullptr;
			color=INVALID;
		}
		rb_tree_node(const rb_tree_node& x):T(x)
		{
			left=nullptr;
			right=nullptr;
			parent=nullptr;
			color=INVALID;
		}
		rb_tree_node(rb_tree_node&& x):T((T&&)x)
		{
			left=x.left;
			right=x.right;
			parent=x.parent;
			color=x.color;
			x.left=nullptr;
			x.right=nullptr;
			x.parent=nullptr;
		}
		friend class rb_tree<T>;
	
};

template <class T>
class rb_tree
{
	private:
		int (*compare_func) (T&, T&) = nullptr;

	public:
		rb_tree_node<T> *root=nullptr;
		rb_tree()=default;
		rb_tree(int(*cmp)(T&,T&)):rb_tree()
		{
			compare_func=cmp;
		}
		rb_tree_node<T> * get_root()
		{
			return root;
		}
		void delete_tree(rb_tree_node<T>* x)
		{
			if(x->left!=nullptr)
				delete_tree(x->left);
			if(x->right!=nullptr)
				delete_tree(x->right);
			delete x;

		}
		~rb_tree()
		{
			if(root!=nullptr)
				delete_tree(root);
			root=nullptr;
		}
		void rotate_left(rb_tree_node<T> *x)
		{
			rb_tree_node<T> *newparent=x->parent;
			rb_tree_node<T> *t=nullptr;
			bool flag=false;
			bool element_left=false;
			if(root==x)
				flag=true;
			if(newparent!=nullptr && x==newparent->left)
				element_left=true;

			if(x->right==nullptr)
				return;
			t=x->right;
			x->right=t->left;
			if(t->left!=nullptr)
				t->left->parent=x;
			t->left=x;
			x->parent=t;
			if(element_left)
				newparent->left=t;
			else
			{
				if(newparent!=nullptr)
					newparent->right=t;
			}
			t->parent=newparent;
			if(flag)
				root=t;
		}
	
		void rotate_right(rb_tree_node<T> *x)
		{
			rb_tree_node<T> *newparent=x->parent;
			rb_tree_node<T> *t=nullptr;
			bool flag=false;
			bool element_right=false;
			if(root==x)
				flag=true;
			if(newparent!=nullptr && x==newparent->left)
				element_right=true;

			if(x->left==nullptr)
				return;
			t=x->left;
			x->left=t->right;
			if(t->right!=nullptr)
				t->right->parent=x;
			t->right=x;
			x->parent=t;
			if(element_right)
				newparent->left=t;
			else
			{
				if(newparent!=nullptr)
					newparent->right=t;
			}
			t->parent=newparent;
			if(flag)
				root=t;
		}

		rb_tree_node<T> *get_uncle(rb_tree_node<T> *x)
		{
			if(x->parent==nullptr)
				return nullptr;
			if(x->parent->parent->left==x->parent)
				return x->parent->parent->right;
			return x->parent->parent->left;
		}
		
		rb_tree_node<T> *get_grandfather(rb_tree_node<T> *x)
		{
			if((x->parent!=nullptr) && (x->parent->parent!=nullptr))
				return x->parent->parent;
			return nullptr;
		}
		
		void balance(rb_tree_node<T> *x)
		{
			rb_tree_node<T> *gr=nullptr;
			rb_tree_node<T> *uncle=nullptr;
			if(x->parent==nullptr)
			{
				x->color=BLACK;
				return;
			}
			else
				if(x->parent->color==BLACK)
					return;

			uncle=get_uncle(x);
			if((uncle!=nullptr) && (uncle->color==RED))
			{
				x->parent->color=BLACK;
				uncle->color=BLACK;
				gr=get_grandfather(x);
				gr->color=RED;
				balance(gr);
			}
			else
			{
				gr=get_grandfather(x);
				if((x==x->parent->right) && (x->parent==gr->left))
				{
					rotate_left(x->parent);
					x=x->left;
				}else
					if((x==x->parent->left) && (x->parent==gr->right))
					{
						rotate_right(x->parent);
						x=x->right;
					}
				gr=get_grandfather(x);
				x->parent->color=BLACK;
				gr->color=RED;
				if((x==x->parent->left) && (x->parent=gr->left))
					rotate_right(gr);
				else
					rotate_left(gr);
			}
		}
			
		void add_node(rb_tree_node<T> *& newroot,rb_tree_node<T> *x,rb_tree_node<T> *parent=nullptr)
		{
			if(newroot==nullptr)
			{
				newroot=x;
				newroot->color=RED;
				newroot->parent=parent;
				balance(x);
			}
			else if(compare_func(*x,*newroot)<0)
				add_node(newroot->left,x,newroot);
			else		
				add_node(newroot->right,x,newroot);
		}

		rb_tree_node<T>* find(rb_tree_node<T> * start,int key)
		{
			rb_tree_node<T>* t=start;
			rb_tree_node<T>* ret=nullptr;
			if(start==nullptr)
				return nullptr;
			if((t->get_phone())==key)
			{
				ret=t;
				return ret;
			}
			if(t->get_right()!=nullptr)
				ret=find(t->get_right(),key);
			if(ret!=nullptr)
				return ret;
			if(t->get_left()!=nullptr)
				ret=find(t->get_left(),key);
			return ret;

		}
		/*int read(const char *name)
		{
			FILE *fp;
			rb_tree_node<T> buf;
			rb_tree_node<T> *curr=nullptr;
			if(!(fp=fopen(name,"r")))
				return ERROR_OPEN1;
			while(buf.read(fp)==SUCCESS)
			{
				curr=new rb_tree_node<T> ((rb_tree_node<T>&&)buf);
				if(!curr)
				{
					fclose(fp);
					return ERROR_READBUF;
				}
				add_node(root,curr,nullptr);
			}
			if(!feof(fp))
			{
				fclose(fp);
				return ERROR_END;
			}
			fclose(fp);
			return SUCCESS;
		}*/

		void print (int r)
		{
			if (root != nullptr)
			{
				print_node(r,0,root);
			}
			else
				printf ("Empty rb_tree\n");
		}

		void print_node(int max_level,int level,rb_tree_node<T> *p)
		{
			int spaces=level*2;
			if(max_level<level)return;
			for(int i=0;i <spaces;i++)
			{
				printf(" ");
			}
			if(p->color==BLACK)
				printf("black ");
			if(p->color==RED)
				printf("red ");
			p->print();
			printf("\n");
			if(p->left)
				print_node(max_level,level+1,p->left);
			if(p->right)
				print_node(max_level,level+1,p->right);
		}

};



#endif //RB_TREE 