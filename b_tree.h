#ifndef BNODE
#define BNODE
#include <stdio.h>
template <class T> class b_tree;
template <class T>
class b_tree_node:public T 
{
	private:
		T *values=nullptr;//2*m
		int size =0; // кол-во элементов в узле
		b_tree_node ** children =nullptr;//2*m+1 elements
	public:
		b_tree_node()=default;
		void set_size(int m1)
		{
			size=m1;
		}
		int get_size()
		{
			return size;
		}
		b_tree_node** get_children()
		{
			return children;
		}
		T* get_values()
		{
			return values;
		}
		int init(int m)
		{
			values= new T[2*m];
			if(values==nullptr)
				return -1;
			children =new b_tree_node<T>*[2*m+1];
			if(children==nullptr)
				return -2;
			for (int i = 0; i <2*m+1; i++)
				children[i]=nullptr;
			size=0;
			return 0;
		}
		~b_tree_node()
		{
			if(values!=nullptr){
				delete [] values;
				values=nullptr;
			}
			if(children!=nullptr){
				delete [] children;
				children=nullptr;
			}
			size=0;
		}
		
		friend class b_tree<T>;
		friend class database;

};
template <class T>
class b_tree
{
	private:
		int m=1;
		
		int (*compare_func) (T&, T&) = nullptr;
	public:
		b_tree_node<T> *root=nullptr;
		b_tree()=default;
		b_tree(int(*cmp)(T&,T&),int m1):b_tree()
		{
			compare_func=cmp;
			m=m1;
		}
		~b_tree()
		{
			if(root!=nullptr)
				freenode(root);
		}
		void init(int m1)
		{
			m=m1;
		}
		int get_m()
		{
			return m;
		}
		int search (T *node, int size,T &x)
		{
			int mid, right, left;
			right = size; 
			left = 0;
			while (right != left)
			{
				mid=(int)((right+left)/2);
				if (compare_func(x,node[mid])>0)
					left = ++mid;
				else
					right = mid;
			}
			return right;
		}
		enum ERRORS
		{
			SUCCESS,
			ERROR_ROOT_INIT,
			ERROR_NEWROOT,
			DELETE_CODE,
			GOOD_RETURN
		};
		int add_node (b_tree_node<T> * pos, T& x)
		{
			int ret = -1;
			T y;
			b_tree_node<T> * first = root;
			b_tree_node<T> * second;
			if (pos == nullptr)
			{ 
				root = new b_tree_node<T>;
				if (root==nullptr)
					return DELETE_CODE;
				ret = root->init(m);
				if (ret != SUCCESS)
				{
					delete root;
					return ERROR_ROOT_INIT;
				}
				root->values[0]=(T&&)x;
				root->size+=1;
				
				return SUCCESS;
			}
			
			ret = add_element (x,y,first,second);
			if (ret==SUCCESS) 
				return SUCCESS;
			if (ret==DELETE_CODE) 
				return ret;
					
			b_tree_node<T> * newroot = new b_tree_node<T>;
			if (newroot==nullptr)
				return ERROR_NEWROOT;
			
			ret = newroot->init(m);
			if (ret != SUCCESS)
			{
				delete newroot;
				return ERROR_NEWROOT;
			}
			
			newroot->values[0] = (T&&)y;
			newroot->children[0] = first;
			newroot->children[1] = second;
			newroot->set_size(1);
			root = newroot;
			
			return SUCCESS;
		}
		
		int add_element(T& x,T& y, b_tree_node<T> *& first, b_tree_node<T> *& second )
		{
			int place=0,ret=-1,i=0;
			place = search(first->values,first->size,x);
			if (first->children[place])
			{
				ret = add_element(x,y,first->children[place],second);
				if (ret == SUCCESS) 
					return SUCCESS;
				
				if (ret == DELETE_CODE) 
					return DELETE_CODE;
			}
			else
			{
				y = (T&&)x;
				second = 0;
				
			}
			
			if (first->size<2*m)
			{
				for (i=first->size;i>place;i--)
				{
					first->values[i]=(T&&)first->values[i-1];
					first->children[i+1]=first->children[i];
				}
				first->children[place+1]=second;
				first->values[place] = (T&&) y;
				first->set_size(first->size+1);
				
				return SUCCESS;
			}
			b_tree_node<T>* t = new b_tree_node<T>;
			
			if (t==nullptr)
				return DELETE_CODE;
			
			ret = t->init(m);
			if (ret != SUCCESS)
			{
				delete t;
				return DELETE_CODE;
			}
			if (place == 2*m)
			{
				t->children[m] = second;
				t->values[m-1] = (T&&) y;
				for (i=(2*m-1);i>m;i--)
				{
					t->values[i-m-1]=(T&&)first->values[i];
					t->children[i-m]=first->children[i+1];
					first->children[i+1] = nullptr;
				}
				t->children[0] = first->children[m+1];
				first->children[m+1] = nullptr;
				y = (T&&) first->values[m];
				second = t;
				first->set_size(m);
				second->set_size(m);
				return GOOD_RETURN;
			}

			if (place>m)
			{
				t->children[m] = first->children[2*m];
				for (i=2*m-1;i>=place;i--)
				{
					t->values[i-m]=(T&&)first->values[i];
					t->children[i-m+1]=first->children[i+1];
					first->children[i+1] = nullptr;
				}
				t->children[place-m] = second;
				t->values[place-m-1] = (T&&) y;
				for (i=place-1;i>m;i--)
				{
					t->values[i-m-1]=(T&&)first->values[i];
					t->children[i-m]=first->children[i+1];
					first->children[i+1] = nullptr;
				}
				t->children[0] = first->children[m+1];
				first->children[m+1] = nullptr;
				y = (T&&) first->values[m];
				second = t;
				first->set_size(m);
				second->set_size(m);
				return GOOD_RETURN;
			}
			if (place == m)
			{
				for (i=2*m-1;i>=m;i--)
				{
					t->values[i-m]=(T&&)first->values[i];
					t->children[i-m+1]=first->children[i+1];
					first->children[i+1] = nullptr;
				}
				t->children[0] = second;
				second = t;
				first->set_size(m);
				second->set_size(m);
				return GOOD_RETURN;
			}
			if (place < m)
			{
				for (i=2*m-1;i>=m;i--)
				{
					t->values[i-m]=(T&&)first->values[i];
					t->children[i-m+1]=first->children[i+1];
					first->children[i+1] = nullptr;
				}
				for (i=m;i>place;i--)
				{
					first->values[i]=(T&&)first->values[i-1];
					first->children[i+1]=first->children[i];
				}
				first->children[place+1]=second;
				first->values[place]=(T&&)y;
				t->children[0] = first->children[m+1];
				first->children[m+1] = nullptr;
				y =(T&&)first->values[m];
		
				second = t;
				first->set_size(m);
				second->set_size(m);
				return GOOD_RETURN;
			}
			return GOOD_RETURN;
			
		}
		/*int read(const char *name)
		{
			FILE *fp;
			T x;
			int ret=0;
			if(!(fp=fopen(name,"r")))
				return 1;
			while(x.read(fp)==SUCCESS)
			{
				ret=add_node(root,x);
				if(ret!=SUCCESS)
				{
					fclose(fp);
					return 2;
				}
			}
			if(!feof(fp))
			{
				fclose(fp);
				return 3;

			}
			fclose(fp);
			return SUCCESS;
		}*/
		void print (int r)
        {
            if (root == nullptr)
                printf ("Empty B tree\n");
            else
                print_to_level (root, 0, r);
        }
        void print_to_level (b_tree_node<T> * node, int level, int max_level)
        {
             int i, j;
            if (level > max_level)
                return;           
            
            if (node == nullptr)
            {
                
                return;
            }
            for (j = 0; j < level; j++)
                printf ("  ");
            for (i = 0; (i < node->size) && (i < max_level); i++)
            {
                printf ("  ");
                node->values[i].print ();
                printf ("  ");
            }
            
            if (i != 0)
            {
                if ((i >= max_level) && (i < node->size))
                    printf ("...\n");
                else
                    printf ("\n");
            }
            if (node->children != nullptr)
            {
                for (i = 0; i <= node->size; i++)
                {
                    print_to_level (node->children[i], level + 1, max_level);
                }
            }
        }

        

       friend class database;

	private:
		void freenode(b_tree_node<T> *x)
		{
			if(x==nullptr)
				return;
			for(int i=0;i<=x->size;i++)
			{
				freenode(x->children[i]);
			}
			delete x;
		}


		
	
};

#endif //BNODE
