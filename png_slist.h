
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_SINGLE_LIST
#define PNG_SINGLE_LIST

#include<iostream>

using namespace std;

struct slist_base_node
{//链表基本节点，只有一个next域
	slist_base_node *next;
};

template <typename T>
struct slist_node :public slist_base_node
{//单链表节点，继承基本节点，增加一个data域
	T data;
};

struct slist_base_iterator
{//单链表基本迭代器，封装一个基本节点指针
	typedef size_t						size_type;
	typedef ptrdiff_t					difference_type;
	typedef forward_iterator_tag		iterator_category;

	slist_base_node *p_node;//基本节点指针

	void incr() { p_node = p_node->next; }
	slist_base_iterator(slist_base_node *rhs) :p_node(rhs){}
	bool operator==(const slist_base_iterator &rhs)
	{
		return p_node == rhs.p_node;
	}
	bool operator!=(const slist_base_iterator &rhs)
	{
		return p_node != rhs.p_node;
	}
};

template <typename T>
struct slist_iterator :public slist_base_iterator
{//单链表迭代器，继承单链表基本迭代器
	typedef slist_iterator<T>			iterator;
	typedef T*							pointer;
	typedef const T*					const_pointer;
	typedef T&							reference;
	typedef const T&					const_reference;
	typedef T							value_type;
	typedef slist_node<T>				node;
	slist_iterator(node *p) :slist_base_iterator(p){}//构造函数，运用了多态性
	slist_iterator() :slist_base_iterator(nullptr){}//nullptr，空指针，C++11新特性
	slist_iterator(const slist_iterator &rhs) :slist_base_iterator(rhs.p_node){}
	//操作符重载
	reference operator*() { return static_cast<node*>(p_node)->data; }
	const_reference operator*()const {return static_cast<node*>(p_node)->data; }
	pointer operator->() { return &(operator*()); }
	const_pointer operator->()const { return &(operator*()); }
	iterator& operator++()
	{
		incr();
		return *this;
	}
	iterator operator++(int)
	{
		iterator tmp = *this;
		++tmp;
		return tmp;
	}
};

template <typename T>
class png_slist
{//单链表
public:
	//诸类型别名
	typedef T									value_type;
	typedef value_type*							pointer;
	typedef T&									reference;
	typedef const value_type*					const_pointer;
	typedef const T&							const_reference;
	typedef size_t								size_type;
	typedef ptrdiff_t							difference_type;
	typedef slist_iterator<T>					iterator;
private:
	typedef slist_node<T>						node;
	typedef slist_base_node						base_node;
	typedef slist_base_iterator					base_iterator;
	static node* createNode(const T &value)
	{//构造一个节点
		node *p = node_alloc.allocate(1);
		node_alloc.construct(&p->data, value);
		p->next = nullptr;
		return p;
	}
	static void destroyNode(node *p)
	{//销毁一个节点
		node_alloc.destroy(&p->data);
		node_alloc.deallocate(p,1);
	}
	base_node* insert_aux(base_node *pre, base_node *cur)
	{//在pre之后插入cur
		cur->next = pre->next;
		pre->next = cur;
		return cur;
	}
	iterator findPre(const T &value)
	{//寻找value前驱
		node *curr = static_cast<node*>(head.next), *pre = &head;
		while (curr != nullptr && curr->data != value)
		{
			pre = curr;
			curr = static_cast<node*>(curr->next);
		}
		return iterator(pre);//返回迭代器
	}
	//数据成员
	node head;
	static allocator<node> node_alloc;//节点内存分配器
public:
	//构造函数
	png_slist(){ head.next = nullptr; head.data = T(); }//默认构造函数
	png_slist(const png_slist &rhs) :png_slist()//C++11新特性，先委托其他构造函数初始化
	{//再执行函数体
		node *curr = static_cast<node*>(rhs.head.next);
		while (curr != nullptr)
		{
			push_front(curr->data);//不断从头插
			curr = static_cast<node*>(curr->next);
		}
	}
	//begin和end操作
	iterator begin() { return iterator(static_cast<node*>(head.next)); }
	iterator end() { return iterator(nullptr); }
	reference front()const { return *begin(); }
	size_type size()
	{
		size_type slist_size = 0;
		for (base_node *p = head.next; p != nullptr; p = p->next)
			++slist_size;
		return slist_size;
	}
	void push_front(const T &value)
	{//从链表头插入
		node *curr = createNode(value);
		insert_aux(&head, curr);
	}
	void pop_front()
	{//删除链表头结点
		node *p = static_cast<node*>(head.next);
		head.next = p->next;
		destroyNode(p);
	}
	void swap(png_slist &rhs)
	{//交换连个链表，其实只是交换头结点而已
		base_node *tmp = head.next;
		head.next = rhs.head.next;
		rhs.head.next = tmp;
	}
	iterator find(const T &value) const
	{//查找一个元素
		node *p = static_cast<node*>(head.next);
		while (p != nullptr && p->data != value)
			p = static_cast<node*>(p->next);
		return iterator(p);
	}
	iterator erase(const T &value)
	{//删除指定元素
		iterator pre = findPre(value), curr = static_cast<node*>(pre.p_node->next);
		pre.p_node->next = curr.p_node->next;
		destroyNode(static_cast<node*>(curr.p_node));
		return iterator(static_cast<node*>(pre.p_node->next));
	}
	iterator insert(iterator p,const T &value)
	{//在迭代器p之前插入value元素
		node *curr = createNode(value);
		iterator pre = findPre(*p);
		insert_aux(pre.p_node, curr);
		return iterator(curr);
	}
	void clear()
	{//清空链表
		node *curr = static_cast<node*>(head.next);
		while (curr != nullptr)
		{
			head.next = static_cast<node*>(curr->next);
			destroyNode(curr);
			curr = static_cast<node*>(head.next);
		}
	}
	~png_slist()
	{//析构
		clear();
	}
};

template <typename T> allocator<slist_node<T>> png_slist<T>::node_alloc;//定义分配器

#endif