
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
{//��������ڵ㣬ֻ��һ��next��
	slist_base_node *next;
};

template <typename T>
struct slist_node :public slist_base_node
{//������ڵ㣬�̳л����ڵ㣬����һ��data��
	T data;
};

struct slist_base_iterator
{//�������������������װһ�������ڵ�ָ��
	typedef size_t						size_type;
	typedef ptrdiff_t					difference_type;
	typedef forward_iterator_tag		iterator_category;

	slist_base_node *p_node;//�����ڵ�ָ��

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
{//��������������̳е��������������
	typedef slist_iterator<T>			iterator;
	typedef T*							pointer;
	typedef const T*					const_pointer;
	typedef T&							reference;
	typedef const T&					const_reference;
	typedef T							value_type;
	typedef slist_node<T>				node;
	slist_iterator(node *p) :slist_base_iterator(p){}//���캯���������˶�̬��
	slist_iterator() :slist_base_iterator(nullptr){}//nullptr����ָ�룬C++11������
	slist_iterator(const slist_iterator &rhs) :slist_base_iterator(rhs.p_node){}
	//����������
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
{//������
public:
	//�����ͱ���
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
	{//����һ���ڵ�
		node *p = node_alloc.allocate(1);
		node_alloc.construct(&p->data, value);
		p->next = nullptr;
		return p;
	}
	static void destroyNode(node *p)
	{//����һ���ڵ�
		node_alloc.destroy(&p->data);
		node_alloc.deallocate(p,1);
	}
	base_node* insert_aux(base_node *pre, base_node *cur)
	{//��pre֮�����cur
		cur->next = pre->next;
		pre->next = cur;
		return cur;
	}
	iterator findPre(const T &value)
	{//Ѱ��valueǰ��
		node *curr = static_cast<node*>(head.next), *pre = &head;
		while (curr != nullptr && curr->data != value)
		{
			pre = curr;
			curr = static_cast<node*>(curr->next);
		}
		return iterator(pre);//���ص�����
	}
	//���ݳ�Ա
	node head;
	static allocator<node> node_alloc;//�ڵ��ڴ������
public:
	//���캯��
	png_slist(){ head.next = nullptr; head.data = T(); }//Ĭ�Ϲ��캯��
	png_slist(const png_slist &rhs) :png_slist()//C++11�����ԣ���ί���������캯����ʼ��
	{//��ִ�к�����
		node *curr = static_cast<node*>(rhs.head.next);
		while (curr != nullptr)
		{
			push_front(curr->data);//���ϴ�ͷ��
			curr = static_cast<node*>(curr->next);
		}
	}
	//begin��end����
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
	{//������ͷ����
		node *curr = createNode(value);
		insert_aux(&head, curr);
	}
	void pop_front()
	{//ɾ������ͷ���
		node *p = static_cast<node*>(head.next);
		head.next = p->next;
		destroyNode(p);
	}
	void swap(png_slist &rhs)
	{//��������������ʵֻ�ǽ���ͷ������
		base_node *tmp = head.next;
		head.next = rhs.head.next;
		rhs.head.next = tmp;
	}
	iterator find(const T &value) const
	{//����һ��Ԫ��
		node *p = static_cast<node*>(head.next);
		while (p != nullptr && p->data != value)
			p = static_cast<node*>(p->next);
		return iterator(p);
	}
	iterator erase(const T &value)
	{//ɾ��ָ��Ԫ��
		iterator pre = findPre(value), curr = static_cast<node*>(pre.p_node->next);
		pre.p_node->next = curr.p_node->next;
		destroyNode(static_cast<node*>(curr.p_node));
		return iterator(static_cast<node*>(pre.p_node->next));
	}
	iterator insert(iterator p,const T &value)
	{//�ڵ�����p֮ǰ����valueԪ��
		node *curr = createNode(value);
		iterator pre = findPre(*p);
		insert_aux(pre.p_node, curr);
		return iterator(curr);
	}
	void clear()
	{//�������
		node *curr = static_cast<node*>(head.next);
		while (curr != nullptr)
		{
			head.next = static_cast<node*>(curr->next);
			destroyNode(curr);
			curr = static_cast<node*>(head.next);
		}
	}
	~png_slist()
	{//����
		clear();
	}
};

template <typename T> allocator<slist_node<T>> png_slist<T>::node_alloc;//���������

#endif