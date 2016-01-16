
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
{//����ڵ㣬˫������
	friend class png_list<T>;
	friend class list_iterator<T>;
	list_node *next;
	list_node *prev;
	T data;
	list_node(const T &d):data(d),next(NULL),prev(NULL){}
};

template <typename T>
class list_iterator
{//���������
public:
	//���ͱ���
	typedef list_iterator<T>				iterator;
	typedef const list_iterator<T>			const_iterator;
	typedef bidirectional_iterator_tag		iterator_category;//����������
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
	node *ptr;//��װ�Ľڵ�ָ��
public:
	list_iterator():ptr(NULL){}
	list_iterator(node *p):ptr(p){}
	list_iterator(const iterator &iter):ptr(iter.ptr){}
	//�����ز�����
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
{//˫��ѭ������
public:
	//���ͱ���
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
	static allocator<node> alloc;//�ڴ������
private:
	node* newNode() {return alloc.allocate(1);}//����һ��node�ռ�
	void deleteNode(node *p) {alloc.deallocate(p,1);}//����һ��node�ռ�
	node* constructNode(const T &d)
	{//����һ���ڵ�
		node *p = newNode();
		new(&(p->data)) T(d);
		return p;
	}
	void destroyNode(node *p)
	{//����һ���ڵ�
		(&(p->data))->~T();
		deleteNode(p);
	}
	void empty_initialized()
	{//��ʼ��������
		head = newNode();
		head->next = head;
		head->prev = head;
	}
	void transfer(iterator p,iterator first,iterator last)
	{//��[first,last)֮���Ԫ�ز��뵽p��ָԪ��ǰ��
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
	//���Ԫ�ز���
	iterator insert(iterator,const T&);
	void insert(iterator,size_type,const T&);
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);
	void push_back(const T &value) {insert(end(),value);}
	void push_front(const T &value) {insert(begin(),value);}
	//transfer�йز���
	void splice(iterator p,png_list &lst)
	{//��lst�����Ԫ�����ӵ�������
		transfer(p,lst.begin(),lst.end());
	}
	void splice(iterator p,png_list &lst,iterator start)
	{//��start��ָԪ�����ӵ�pǰ
		iterator last = start;
		++last;
		if(p == start || p == last) return ;
		transfer(p,start,last);
	}
	void splice(iterator p,png_list &lst,iterator start,iterator last)
	{//��lst����[start,last)��Ԫ�����ӵ�pǰ
		if(start != last)
		{
			transfer(p,start,last);
		}
	}
	void merge(png_list &);//�ϲ���������
	void reverse();//��������
	void sort();//����
	//���캯��
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
	//begin��end����
	iterator begin() {return head->next;}
	const_iterator begin()const {return head->next;}
	iterator end() {return head;}
	const_iterator end()const {return head;}
	//������С����
	size_type size()const {return distance(begin(),end());}
	bool empty()const {return head == head->next;}
	void resize(size_type,const T& = T());
	//��������Ԫ�ز���
	reference back() {return *(--end());}
	const_reference back()const {return *(--end());}
	reference front() {return *(begin());}
	const_reference front()const {return *(begin());}
	//ɾ��Ԫ�ز���
	iterator erase(iterator,iterator);
	iterator erase(iterator);
	void clear()
	{//�������
		erase(begin(),end());
		head->next = head;
		head->prev = head;
	}
	void pop_back() {erase(--end());}
	void pop_front() {erase(begin());}
	//�����ĸ�ֵ����
	png_list& operator=(const png_list &lst)
	{
		clear();
		insert(begin(),lst.begin(),lst.end());
		return *this;
	}
	void swap(png_list &lst)
	{//������������
		iterator tmp = head;
		head = lst.head;
		lst.head = tmp.ptr;
	}
	template <typename InputIterator>
	void assign(InputIterator b, InputIterator e)
	{//��[b,e)��ΧԪ����������
		clear();
		insert(begin(),b,e);
	}
	void assign(size_type n,const T &value)
	{//����������Ϊn��value
		clear();
		insert(begin(),n,value);
	}
	~png_list()
	{//����
		if(!empty())
		{
			clear();
			destroyNode(head);
		}
	}
};

template <typename T> allocator<typename png_list<T>::node> png_list<T>::alloc;//�����ڴ������

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
	{//����sizeС����������ɾ��һ����Ԫ��
		iterator start = begin();
		advance(start,n);
		erase(start,end());
	}
	else insert(end(),n - length,value);//�������һ����Ԫ��
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

