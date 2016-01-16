
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#include<iostream>

using namespace std;
template <typename T> class png_list;
template <typename T> class list_iterator;

template <typename T>
class list_node
{//链表节点，双向链表
	friend class png_list<T>;
	friend class list_iterator<T>;
	list_node *next;
	list_node *prev;
	T data;
	list_node(const T &d):data(d),next(NULL),prev(NULL){}
};

template <typename T>
class list_iterator
{//链表迭代器
public:
	//类型别名
	typedef list_iterator<T>				iterator;
	typedef const list_iterator<T>			const_iterator;
	typedef bidirectional_iterator_tag		iterator_category;//迭代器类型
	typedef T							value_type;
	typedef T*							pointer;
	typedef const T*						const_pointer;
	typedef T&							reference;
	typedef const T&						const_reference;
	typedef size_t						size_type;
	typedef ptrdiff_t						difference_type;
	typedef list_node<T>					node;
private:
	friend class png_list<T>;
	node *ptr;//封装的节点指针
public:
	list_iterator():ptr(NULL){}
	list_iterator(node *p):ptr(p){}
	list_iterator(const iterator &iter):ptr(iter.ptr){}
	//各重载操作符
	list_iterator& operator=(const iterator &iter)
	{
		ptr = iter.ptr;
		return *this;
	}
	bool operator==(const iterator &iter) { return ptr == iter.ptr;}
	bool operator!=(const iterator &iter) { return ptr != iter.ptr;}
	reference operator*() { return (*ptr).data;}
	const_reference operator*()const { return (*ptr).data;}

	pointer operator->() { return &(ptr->data);}
	const_pointer operator->()const { return &(ptr->data);}

	iterator& operator++()
	{
		ptr = (*ptr).next;
		return *this;
	}
	iterator operator++(int)
	{
		iterator tmp = *this;
		++*this;
		return tmp;
	}
	iterator& operator--()
	{
		ptr = (*ptr).prev;
		return *this;
	}
	iterator operator--(int)
	{
		iterator tmp = *this;
		--*this;
		return tmp;
	}
};

template <typename T>
class png_list
{//双向循环链表
public:
	//类型别名
	typedef list_iterator<T>			iterator;
	typedef const list_iterator<T>		const_iterator;
	typedef T&							reference;
	typedef const T&					const_reference;
	typedef size_t						size_type;
	typedef ptrdiff_t					difference_type;
	typedef T							value_type;
	typedef list_node<T>				node;
	typedef T*							pointer;
	typedef const T*					const_pointer;
private:
	node *head;
	static allocator<node> alloc;//内存分配器
private:
	node* newNode() {return alloc.allocate(1);}//分配一个node空间
	void deleteNode(node *p) {alloc.deallocate(p,1);}//回收一个node空间
	node* constructNode(const T &d)
	{//构造一个节点
		node *p = newNode();
		new(&(p->data)) T(d);
		return p;
	}
	void destroyNode(node *p)
	{//销毁一个节点
		(&(p->data))->~T();
		deleteNode(p);
	}
	void empty_initialized()
	{//初始化空链表
		head = newNode();
		head->next = head;
		head->prev = head;
	}
	void transfer(iterator p,iterator first,iterator last)
	{//将[first,last)之间的元素插入到p所指元素前面
		if(p != last)
		{
			last.ptr->prev->next = p.ptr;
			first.ptr->prev->next = last.ptr;
			iterator tmp = p.ptr->prev;
			tmp.ptr->next = first.ptr;
			p.ptr->prev = last.ptr->prev;
			last.ptr->prev = first.ptr->prev;
			first.ptr->prev = tmp.ptr;
		}
	}
public:
	//添加元素操作
	iterator insert(iterator,const T&);
	void insert(iterator,size_type,const T&);
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);
	void push_back(const T &value) {insert(end(),value);}
	void push_front(const T &value) {insert(begin(),value);}
	//transfer有关操作
	void splice(iterator p,png_list &lst)
	{//将lst链表的元素连接到本链表
		transfer(p,lst.begin(),lst.end());
	}
	void splice(iterator p,png_list &lst,iterator start)
	{//将start所指元素链接到p前
		iterator last = start;
		++last;
		if(p == start || p == last) return ;
		transfer(p,start,last);
	}
	void splice(iterator p,png_list &lst,iterator start,iterator last)
	{//将lst链表[start,last)的元素链接到p前
		if(start != last)
		{
			transfer(p,start,last);
		}
	}
	void merge(png_list &);//合并两个链表
	void reverse();//逆置链表
	void sort();//排序
	//构造函数
	png_list() {empty_initialized();}
	png_list(const png_list &lst)
	{
		empty_initialized();
		insert(begin(),lst.begin(),lst.end());
	}
	template <typename InputIterator>
	png_list(InputIterator b, InputIterator e)
	{
		empty_initialized();
		insert(begin(),b,e);
	}
	png_list(size_type n,const T &value = T())
	{
		empty_initialized();
		insert(begin(),n,value);
	}
	//begin和end操作
	iterator begin() {return head->next;}
	const_iterator begin()const {return head->next;}
	iterator end() {return head;}
	const_iterator end()const {return head;}
	//容器大小操作
	size_type size()const {return distance(begin(),end());}
	bool empty()const {return head == head->next;}
	void resize(size_type,const T& = T());
	//访问容器元素操作
	reference back() {return *(--end());}
	const_reference back()const {return *(--end());}
	reference front() {return *(begin());}
	const_reference front()const {return *(begin());}
	//删除元素操作
	iterator erase(iterator,iterator);
	iterator erase(iterator);
	void clear()
	{//请空链表
		erase(begin(),end());
		head->next = head;
		head->prev = head;
	}
	void pop_back() {erase(--end());}
	void pop_front() {erase(begin());}
	//容器的赋值操作
	png_list& operator=(const png_list &lst)
	{
		clear();
		insert(begin(),lst.begin(),lst.end());
		return *this;
	}
	void swap(png_list &lst)
	{//交换两个链表
		iterator tmp = head;
		head = lst.head;
		lst.head = tmp.ptr;
	}
	template <typename InputIterator>
	void assign(InputIterator b, InputIterator e)
	{//用[b,e)范围元素重置链表
		clear();
		insert(begin(),b,e);
	}
	void assign(size_type n,const T &value)
	{//将链表重置为n个value
		clear();
		insert(begin(),n,value);
	}
	~png_list()
	{//析构
		if(!empty())
		{
			clear();
			destroyNode(head);
		}
	}
};

template <typename T> allocator<typename png_list<T>::node> png_list<T>::alloc;//定义内存分配器

template <typename T>
typename png_list<T>::iterator png_list<T>::insert(iterator p,const T &value)
{
	node *curr = constructNode(value);
	curr->next = p.ptr;
	p.ptr->prev->next = curr;
	curr->prev = p.ptr->prev;
	p.ptr->prev = curr;
	return curr;
}

template <typename T>
void png_list<T>::insert(iterator p,size_type n,const T &value)
{
	for(size_type i = 0;i != n;++i)
		insert(p,value);
}

template <typename T> template <typename InputIterator>
void png_list<T>::insert(iterator p, InputIterator b, InputIterator e)
{
	for (InputIterator start = b; start != e; ++start)
		insert(p,*start);
}

template <typename T>
void png_list<T>::resize(size_type n,const T &value = T())
{
	size_type length = size();
	if(n <= length)
	{//若新size小于链表长，则删掉一部分元素
		iterator start = begin();
		advance(start,n);
		erase(start,end());
	}
	else insert(end(),n - length,value);//否则插入一部分元素
}

template <typename T>
typename png_list<T>::iterator png_list<T>::erase(iterator p)
{
	if(p != end())
	{
		p.ptr->prev->next = p.ptr->next;
		p.ptr->next->prev = p.ptr->prev;
		iterator tmp = p.ptr->next;
		destroyNode(p.ptr);
		return tmp;
	}
	return p;
}

template <typename T>
typename png_list<T>::iterator png_list<T>::erase(iterator start,iterator last)
{
	iterator first = start.ptr->prev;
	while(start != last)
	{
		iterator tmp = start;
		++start;
		destroyNode(tmp.ptr);
	}
	first.ptr->next = last.ptr;
	last.ptr->prev = first.ptr;
	return last;
}

template <typename T>
void png_list<T>::merge(png_list & lst)
{
	iterator first1 = begin(),last1 = end(),
		first2 = lst.begin(),last2 = lst.end();
	while(first1 != last1 && first2 != last2)
	{
		if(*first2 < *first1)
		{
			iterator tmp = first2;
			++first2;
			transfer(first1,tmp,first2);
		}
		else ++first1;
	}
	if(first2 != last2)
		transfer(last1,first2,last2);
}

template <typename T>
void png_list<T>::reverse()
{
	if(head->next == head || head->next->next == head)
		return ;
	iterator first = ++begin(),last = end();
	while(first != last)
	{
		iterator old_first = first;
		++first;
		transfer(begin(),old_first,first);
	}
}

template <typename T>
void png_list<T>::sort()
{
	if(head->next == head || head->next->next == head)
		return ;
	png_list carry,counter[64];
	int fill = 0;
	while(!empty())
	{
		carry.splice(carry.begin(),*this,begin());
		int i = 0;
		while(i < fill && !counter[i].empty())
		{
			counter[i].merge(carry);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if(i == fill) ++fill;
	}
	for(int i = 1;i < fill;++i)
		counter[i].merge(counter[i - 1]);
	swap(counter[fill - 1]);
}

