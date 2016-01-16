
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_HASHTABLE_H
#define PNG_HASHTABLE_H

//hash��
#include<iostream>
#include<vector>
#include<utility>
#include<functional>

using namespace std;
//��ϣ�����������Ͳ�������Ϊ����ֵ��ֵ���á�ֵָ�롢��ϣ������ȡ�����������ȽϺ��������к�����һ��Ϊ��������
template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey> class hashtable;

template <typename T>
struct hashtable_list_node
{//��ϣ��ڵ㣬������ڵ�
	T value;
	hashtable_list_node *next;
	hashtable_list_node(const T &v) :value(v), next(nullptr){}
};

//1������Ref��Ptr��Ҫ��Ϊ�˱���const���͵Ĵ�����Ϊ�ñ���Ϊ�ײ����ʵ��hash_set��hash_map��ǰ��ֵ���ɱ䣬���߿ɱ�
template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey>
struct hashtable_iterator
{//��ϣ�������
	typedef forward_iterator_tag						iterator_category;//���������ͣ�ǰ�������
	typedef Value										value_type;
	typedef size_t										size_type;
	typedef ptrdiff_t									difference_type;
	typedef Ref											reference;
	typedef Ptr											pointer;

	//2�����ֵ������ı������������ļ�ʹ�ã���ʵ�ֵ���hash_set����Ref��Ptr�ֱ�Ϊconst Value&��const Value*����const������
	//3������hash_map����ΪValue&��Value*����const����������ͨ����ı�ֵ��
	typedef hashtable_iterator<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>	self;
	typedef hashtable<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>					hashtable;
	//�ṩ�ⲿʹ�ã����������ļ��Լ���������(��hash_set)��ʵ��
	typedef hashtable_iterator<Key, Value, Value&, Value*, HashFunction, GetKey, CompareKey>		iterator;
	typedef hashtable_iterator<Key, Value, const Value&, const Value*, HashFunction, GetKey, CompareKey>	const_iterator;
	typedef hashtable_list_node<Value>								node;
	//��ϣ���������װ��������Ա
	const hashtable *ht_ptr;//�ù�ϣ��ָ��
	node *curr;//ָ��Ľڵ�

	hashtable_iterator() :curr(nullptr), ht_ptr(nullptr){}
	hashtable_iterator(node *cur, const hashtable *ht) :curr(cur), ht_ptr(ht){}
	//����������
	reference operator*()const { return curr->value; }
	pointer operator->()const { return &(operator*()); }
	bool operator==(const hashtable_iterator &rhs)const{ return curr == rhs.curr; }
	bool operator!=(const hashtable_iterator &rhs)const{ return curr != rhs.curr; }
	self& operator++()
	{//ǰ����
		node *old_curr = curr;
		curr = curr->next;
		if (curr == nullptr)
		{//���ѵ�������β������Ѱ��һ���ǿ���������curr��Ϊ�׽ڵ��ַ
			size_type bucket_num = ht_ptr->bucket_number(old_curr->value);
			while (curr == nullptr && ++bucket_num != ht_ptr->bucket_count())
				curr = ht_ptr->buckets[bucket_num];
		}
		return *this;
	}
	self operator++(int)
	{
		self tmp = *this;
		++*this;
		return tmp;
	}
};

template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey>
class hashtable
{//��ϣ����
public:
	typedef hashtable_iterator<Key, Value, Value&, Value*, HashFunction, GetKey, CompareKey>	iterator;
	typedef hashtable_iterator<Key, Value, const Value&, const Value*, HashFunction, GetKey, CompareKey> const_iterator;
	typedef hashtable_iterator<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey> self;

	typedef size_t													size_type;
	typedef ptrdiff_t												difference_type;
	typedef Key														key_type;
	typedef Value													value_type;
	typedef HashFunction											hasher;
	typedef CompareKey												key_equal;
	typedef Value*													pointer;
	typedef const Value*											const_pointer;
	typedef Value&													reference;
	typedef const Value&											const_reference;

	typedef hashtable_list_node<Value>								node;
private:
	size_t element_size;//�ڵ�����
	hasher hash;//��ϣ��
	GetKey get_key;//ȡ����
	key_equal equal;//���Ƚ���
	vector<node*> buckets;//��ϣ��
	static allocator<node> node_alloc;//�ڵ�ռ������

	node* new_node(const value_type &value)
	{//����һ���ڵ�
		node *p = node_alloc.allocate(1);
		new (&(p->value)) Value(value);//��λnew���ʽ
		return p;
	}
	void delete_node(node *p)
	{//����һ���ڵ�
		(&(p->value))->~Value();//��ʽ����ֵ����������
		node_alloc.deallocate(p, 1);
	}
	//hash��أ�ȡ��
	size_t bucket_number(const value_type &v)const
	{
		return bucket_number_key(get_key(v), buckets.size());
	}
	size_t bucket_number(const value_type &v, size_t n)const
	{
		return bucket_number_key(get_key(v), n);
	}
	size_t bucket_number_key(const key_type &key)const
	{
		return bucket_number_key(key, buckets.size());
	}
	size_t bucket_number_key(const key_type &key, size_t n)const
	{
		return hash(key) % n;
	}
	//Ѱ��ͬһ������ĳԪ�ص�ǰ������boolΪ�棬��ʾ�βε�������ָԪ����������Ԫ�أ�pair��first��Ա��ָ������
	//����pair��first��Աָ����ǰ��
	pair<self, bool> find_pre(const self&)const;
	friend bool operator==<>(const hashtable&, const hashtable&);//����==��������Ϊ��ϣ����Ԫ
	friend struct hashtable_iterator<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>;
public:
	hashtable(size_t e, const hasher &h, const key_equal &ke) :element_size(0),
		hash(h), equal(ke), get_key(GetKey()) {
		buckets.resize(e);
	}
	hashtable(const hashtable &rhs) :hash(rhs.hash), get_key(rhs.get_key), equal(rhs.equal)
	{
		copy_from(rhs);
	}
	hashtable& operator=(const hashtable&);
	//���ʳ�Ա����
	hasher hash_function()const{ return hash; }
	key_equal get_equal()const{ return equal; }
	void copy_from(const hashtable&);//���βι�ϣ��Ԫ�ظ��ƹ���
	//begin��end����
	self begin()const
	{
		for (size_t i = 0; i != buckets.size(); ++i)
			if (buckets[i]) return self(buckets[i], this);
		return self(nullptr, this);
	}
	self end()const { return self(nullptr, this); }
	//�������
	pair<self, bool> insert_unique(const value_type&);//�����ظ�����ͬһ��ֵ
	self insert_equal(const value_type&);//���ظ�����ͬһ��ֵ
	template<class InputIterator>
	void insert_unique(InputIterator beg, InputIterator end)
	{
		for (InputIterator ii = beg; ii != end; ++ii)
			insert_unique(*ii);
	}
	template <class InputIterator>
	void insert_equal(InputIterator beg, InputIterator end)
	{
		for (InputIterator ii = beg; ii != end; ++ii)
			insert_equal(*ii);
	}
	//ɾ������
	size_t erase(const key_type&);
	void erase(self);
	void erase(self, self);
	void clear()
	{
		erase(begin(), end());
	}
	//���Ҳ���
	self low_bound(const key_type&)const;//�ü���һ�����ֵ�λ��
	self high_bound(const key_type&)const;//�ü����һ�����ֵ�λ�õ���һλ��
	self find(const key_type&)const;
	pair<self, self> equal_range(const key_type &key)const
	{
		return pair<self, self>(low_bound(key), high_bound(key));
	}
	//��������
	size_t bucket_count()const { return buckets.size(); }//Ͱ����
	size_t size()const { return element_size; }//����Ԫ�ظ���
	size_t count(const key_type&)const;
	bool empty()const { return element_size == 0; }
	void resize(size_t);
	size_t element_in_bucket(size_t)const;//����Ͱ��Ԫ������
	void swap(hashtable&);
	void bucket_print()const;
	~hashtable()
	{
		clear();
	}
};
//����ڵ������
template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey>
allocator<hashtable_list_node<Value>> hashtable<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>::node_alloc;

template<class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
hashtable<K, V, Ref, Ptr, HF, GK, CK>& hashtable<K, V, Ref, Ptr, HF, GK, CK>::operator=(const hashtable &rhs)
{
	if (&rhs != this)
	{
		hash = rhs.hash;
		get_key = rhs.get_key;
		equal = rhs.equal;
		copy_from(rhs);
	}
	return *this;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
pair<typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self, bool>
hashtable<K, V, Ref, Ptr, HF, GK, CK>::find_pre(const self &p)const
{
	size_t bkt_num = bucket_number(*p);
	if (buckets[bkt_num] == p.curr)//��pָ��������һ��Ԫ�أ�����ǰ��
		return pair<self, bool>(p, true);
	else
	{//����
		node *cur = buckets[bkt_num];
		while (cur->next != p.curr)
			cur = cur->next;
		return pair<self, bool>(self(cur, this), false);
	}
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::copy_from(const hashtable &rhs)
{//����rhs�е�Ԫ��
	clear();//��չ�ϣ��
	buckets.resize(rhs.buckets.size());
	for (size_t i = 0; i != rhs.buckets.size(); ++i)
	{//ɨ������Ͱ
		if (rhs.buckets[i])
		{//���Ʒǿյ�Ͱ�����������β�巨
			node *curr = rhs.buckets[i], *copy_curr;
			buckets[i] = new_node(curr->value);
			copy_curr = buckets[i];//���²���λ��
			while (curr->next != nullptr)
			{
				copy_curr->next = new_node(curr->next->value);//����������
				curr = curr->next; copy_curr = copy_curr->next;
			}
			copy_curr->next = curr->next;
		}
	}
	element_size = rhs.element_size;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
bool operator==(const hashtable<K, V, Ref, Ptr, HF, GK, CK> &lhs, const hashtable<K, V, Ref, Ptr, HF, GK, CK> &rhs)
{//�ȽϹ�ϣ���Ƿ����
	if (lhs.element_size != rhs.element_size ||
		lhs.bucket_count() != rhs.bucket_count()) return false;//�����ж�Ԫ�ظ�����Ͱ����
	for (size_t i = 0; i != lhs.bucket_count(); ++i)
	{//���ж�ÿ��Ͱ���Ԫ���Ƿ�һ��
		hashtable_list_node<V> *lhs_curr = lhs.buckets[i], *rhs_curr = rhs.buckets[i];
		while (lhs_curr && rhs_curr &&
			lhs.equal(lhs.get_key(lhs_curr->value), rhs.get_key(rhs_curr->value)))
		{
			lhs_curr = lhs_curr->next;
			rhs_curr = rhs_curr->next;
		}
		if (lhs_curr || rhs_curr) return false;
	}
	return true;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
pair<typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self, bool>
hashtable<K, V, Ref, Ptr, HF, GK, CK>::insert_unique(const value_type &v)
{//���ظ�����
	resize(element_size + 1);//�ж��Ƿ���Ҫ���·����ϣ��
	size_t bkt_num = bucket_number(v);
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		if (equal(get_key(curr->value), get_key(v)))//��������Ԫ���Ѵ���
			return pair<self, bool>(self(curr, this), false);//�����ʧ�ܣ�first��Աָ���Ԫ��
	node *p_insert_node = new_node(v);
	p_insert_node->next = buckets[bkt_num];
	buckets[bkt_num] = p_insert_node;
	++element_size;
	return pair<self, bool>(self(p_insert_node, this), true);//������룬firstָ���²���Ԫ��
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self hashtable<K, V, Ref, Ptr, HF, GK, CK>::insert_equal(const value_type &v)
{//���ظ�����
	resize(element_size + 1);
	size_t bkt_num = bucket_number(v);
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		if (equal(get_key(curr->value), get_key(v)))
		{//�ҵ�����ͬ��Ԫ�أ����У�������Ų���
			node *p_insert_node = new_node(v);
			p_insert_node->next = curr->next;
			curr->next = p_insert_node;
			++element_size;
			return self(p_insert_node, this);
		}
	node *p_insert_node = new_node(v);
	p_insert_node->next = buckets[bkt_num];
	buckets[bkt_num] = p_insert_node;
	++element_size;
	return self(p_insert_node, this);//����������ͷ����
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
size_t hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(const key_type &key)
{//ɾ����Ϊkey������Ԫ��
	pair<self, self> low_high = equal_range(key);//ȡ�øü���Χ
	size_t num = 0;
	for (self beg = low_high.first; beg != low_high.second; ++beg)
		++num;
	erase(low_high.first, low_high.second);
	return num;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(self p)
{//ɾ��ָ��λ�õ�Ԫ��
	pair<self, bool> pre = find_pre(p);//������ǰ��
	if (pre.second)
	{//����ĳ������Ԫ��
		size_t bkt_num = bucket_number(*p);
		buckets[bkt_num] = p.curr->next;
		delete_node(p.curr);
	}
	else
	{//����
		pre.first.curr->next = p.curr->next;
		delete_node(p.curr);
	}
	--element_size;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(self first, self last)
{//ɾ��һ��Ԫ��
	if (first == last) return;
	size_t first_bkt_num = bucket_number(*first), last_bkt_num, num = 0;//first����Ͱ�ӱ��
	if (last != end()) last_bkt_num = bucket_number(*last);//last����Ͱ�ӱ��
	else last_bkt_num = buckets.size() - 1;
	//��boolΪ�棬��first��ָ�Ǹ�����һ��Ԫ�أ���ǰ����pre��first��Աָ��first��ָԪ�أ�����ָ��firstǰ��
	pair<self, bool> pre = find_pre(first);
	for (size_t i = first_bkt_num + 1; i < last_bkt_num; ++i)
	{//��ɾ��first����Ͱ��last����Ͱ֮���Ԫ��
		node *r = buckets[i];
		while (r != nullptr)
		{
			node *p = r;
			r = r->next;
			delete_node(p);
			++num;
		}
		buckets[i] = r;
	}
	node *r = first.curr;
	while (r != nullptr && r != last.curr)
	{//ɾ��first����Ͱ��first��ʼ��Ԫ�أ�ֱ������last����ɾ�����
		node *p = r;
		r = r->next;
		delete_node(p);
		++num;
	}
	if (pre.second)//����first�Ƿ��Ǹ�����һ��Ԫ������Ͱ�ڵ�ֵ
		buckets[first_bkt_num] = r;
	else pre.first.curr->next = r;
	if (first_bkt_num != last_bkt_num)
	{//��first��last���ڵ�Ͱ��ͬ����Ҫɾ��last��������last֮ǰ������Ԫ��
		node *r = buckets[last_bkt_num];
		while (r != last.curr)
		{
			node *p = r;
			r = r->next;
			delete_node(p);
			++num;
		}
		buckets[last_bkt_num] = r;
	}
	element_size -= num;//�޸�Ԫ�ظ���
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::low_bound(const key_type &key)const
{//�ҵ���Ϊkey�ĵ�һ��Ԫ��
	size_t bkt_num = bucket_number(key);
	node *curr = buckets[bkt_num];
	while (curr != nullptr && !equal(get_key(curr->value), key))
		curr = curr->next;
	return self(curr, this);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::high_bound(const key_type &key)const
{//�ҵ���λkey�����һ��Ԫ�أ���������һλ��
	size_t bkt_num = bucket_number(key);
	self low = low_bound(key);
	while (low != end() && equal(get_key(low.curr->value), key))
		++low;
	return low;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::find(const key_type &key)const
{//���Ҽ�λkey��Ԫ��
	size_t bkt_num = bucket_number(key);
	node *curr;
	for (curr = buckets[bkt_num]; curr && !equal(get_key(curr->value), key); curr = curr->next);
	return self(curr, this);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
size_t hashtable<K, V, Ref, Ptr, HF, GK, CK>::count(const key_type &key)const
{//ͳ�Ƽ�Ϊkey��Ԫ�صĸ���
	pair<self, self> low_high = equal_range(key);
	size_t cnt = 0;
	for (self first = low_high.first; first != low_high.second; ++first)
		++cnt;
	return cnt;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::resize(size_t new_element_size)
{//���ù�ϣ���С
	if (new_element_size > buckets.size())
	{//Ԫ����������Ͱ����������
		vector<node*> new_buckets(2 * new_element_size, nullptr);//������ʱvector��ΪԪ�ظ���������
		for (size_t i = 0; i != buckets.size(); ++i)
		{//��������
			node *curr = buckets[i];
			while (curr != nullptr)
			{//�������գ����ϲ���ͷ�巨����Ԫ�ز�����ʱvector
				buckets[i] = curr->next;
				size_t bkt_num = bucket_number(curr->value, 2 * new_element_size);
				curr->next = new_buckets[bkt_num];
				new_buckets[bkt_num] = curr;
				curr = buckets[i];
			}
		}
		buckets.swap(new_buckets);
	}
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
size_t hashtable<K, V, Ref, Ptr, HF, GK, CK>::element_in_bucket(size_t bkt_num)const
{//ͳ��ĳ��Ͱ��Ԫ������
	size_t num = 0;
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		++num;
	return num;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::swap(hashtable &rhs)
{//����������ϣ��
	std::swap(hash, rhs.hash);
	std::swap(equal, rhs.equal);
	std::swap(get_key, rhs.get_key);
	std::swap(element_size, rhs.element_size);
	buckets.swap(rhs.buckets);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::bucket_print()const
{//��ӡ��ϣ����ÿ��Ͱ��ӡ
	for (size_t i = 0; i != buckets.size(); ++i)
	{
		if (buckets[i])
		{
			cout << i << " : ";
			node *curr = buckets[i];
			while (curr != nullptr)
			{
				cout << curr->value << ' ';
				curr = curr->next;
			}
			cout << endl;
		}
	}
}

#endif