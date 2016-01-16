
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_HASHTABLE_H
#define PNG_HASHTABLE_H

//hash表
#include<iostream>
#include<vector>
#include<utility>
#include<functional>

using namespace std;
//哈希表声明，类型参数依次为键、值、值引用、值指针、哈希函数、取键函数、键比较函数，其中后三者一般为函数对象
template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey> class hashtable;

template <typename T>
struct hashtable_list_node
{//哈希表节点，单链表节点
	T value;
	hashtable_list_node *next;
	hashtable_list_node(const T &v) :value(v), next(nullptr){}
};

//1、引入Ref和Ptr主要是为了便于const类型的处理，因为该表将作为底层机制实现hash_set和hash_map，前者值不可变，后者可变
template <class Key, class Value, class Ref, class Ptr, class HashFunction, class GetKey, class CompareKey>
struct hashtable_iterator
{//哈希表迭代器
	typedef forward_iterator_tag						iterator_category;//迭代器类型，前向迭代器
	typedef Value										value_type;
	typedef size_t										size_type;
	typedef ptrdiff_t									difference_type;
	typedef Ref											reference;
	typedef Ptr											pointer;

	//2、该种迭代器的别名，留作本文件使用，若实现的是hash_set，则Ref和Ptr分别为const Value&和const Value*，即const迭代器
	//3、若是hash_map，则为Value&和Value*，非const迭代器，可通过其改变值。
	typedef hashtable_iterator<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>	self;
	typedef hashtable<Key, Value, Ref, Ptr, HashFunction, GetKey, CompareKey>					hashtable;
	//提供外部使用，包括程序文件以及其他类型(如hash_set)的实现
	typedef hashtable_iterator<Key, Value, Value&, Value*, HashFunction, GetKey, CompareKey>		iterator;
	typedef hashtable_iterator<Key, Value, const Value&, const Value*, HashFunction, GetKey, CompareKey>	const_iterator;
	typedef hashtable_list_node<Value>								node;
	//哈希表迭代器封装了两个成员
	const hashtable *ht_ptr;//该哈希表指针
	node *curr;//指向的节点

	hashtable_iterator() :curr(nullptr), ht_ptr(nullptr){}
	hashtable_iterator(node *cur, const hashtable *ht) :curr(cur), ht_ptr(ht){}
	//操作符重载
	reference operator*()const { return curr->value; }
	pointer operator->()const { return &(operator*()); }
	bool operator==(const hashtable_iterator &rhs)const{ return curr == rhs.curr; }
	bool operator!=(const hashtable_iterator &rhs)const{ return curr != rhs.curr; }
	self& operator++()
	{//前自增
		node *old_curr = curr;
		curr = curr->next;
		if (curr == nullptr)
		{//若已到该链表尾，则找寻下一个非空链表，并将curr设为首节点地址
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
{//哈希表定义
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
	size_t element_size;//节点数量
	hasher hash;//哈希器
	GetKey get_key;//取键器
	key_equal equal;//键比较器
	vector<node*> buckets;//哈希表
	static allocator<node> node_alloc;//节点空间分配器

	node* new_node(const value_type &value)
	{//构造一个节点
		node *p = node_alloc.allocate(1);
		new (&(p->value)) Value(value);//定位new表达式
		return p;
	}
	void delete_node(node *p)
	{//销毁一个节点
		(&(p->value))->~Value();//显式调用值的析构函数
		node_alloc.deallocate(p, 1);
	}
	//hash相关，取键
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
	//寻找同一链表中某元素的前驱，若bool为真，表示形参迭代器所指元素是链表首元素，pair的first成员亦指向它；
	//否则pair的first成员指向其前驱
	pair<self, bool> find_pre(const self&)const;
	friend bool operator==<>(const hashtable&, const hashtable&);//重载==操作符，为哈希表友元
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
	//访问成员操作
	hasher hash_function()const{ return hash; }
	key_equal get_equal()const{ return equal; }
	void copy_from(const hashtable&);//将形参哈希表元素复制过来
	//begin和end操作
	self begin()const
	{
		for (size_t i = 0; i != buckets.size(); ++i)
			if (buckets[i]) return self(buckets[i], this);
		return self(nullptr, this);
	}
	self end()const { return self(nullptr, this); }
	//插入操作
	pair<self, bool> insert_unique(const value_type&);//不可重复插入同一个值
	self insert_equal(const value_type&);//可重复插入同一个值
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
	//删除操作
	size_t erase(const key_type&);
	void erase(self);
	void erase(self, self);
	void clear()
	{
		erase(begin(), end());
	}
	//查找操作
	self low_bound(const key_type&)const;//该键第一个出现的位置
	self high_bound(const key_type&)const;//该键最后一个出现的位置的下一位置
	self find(const key_type&)const;
	pair<self, self> equal_range(const key_type &key)const
	{
		return pair<self, self>(low_bound(key), high_bound(key));
	}
	//容量操作
	size_t bucket_count()const { return buckets.size(); }//桶个数
	size_t size()const { return element_size; }//返回元素个数
	size_t count(const key_type&)const;
	bool empty()const { return element_size == 0; }
	void resize(size_t);
	size_t element_in_bucket(size_t)const;//给定桶的元素数量
	void swap(hashtable&);
	void bucket_print()const;
	~hashtable()
	{
		clear();
	}
};
//定义节点分配器
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
	if (buckets[bkt_num] == p.curr)//若p指向该链表第一个元素，则无前驱
		return pair<self, bool>(p, true);
	else
	{//否则
		node *cur = buckets[bkt_num];
		while (cur->next != p.curr)
			cur = cur->next;
		return pair<self, bool>(self(cur, this), false);
	}
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::copy_from(const hashtable &rhs)
{//复制rhs中的元素
	clear();//清空哈希表
	buckets.resize(rhs.buckets.size());
	for (size_t i = 0; i != rhs.buckets.size(); ++i)
	{//扫描所有桶
		if (rhs.buckets[i])
		{//复制非空的桶，采用链表的尾插法
			node *curr = rhs.buckets[i], *copy_curr;
			buckets[i] = new_node(curr->value);
			copy_curr = buckets[i];//记下插入位置
			while (curr->next != nullptr)
			{
				copy_curr->next = new_node(curr->next->value);//在其后面插入
				curr = curr->next; copy_curr = copy_curr->next;
			}
			copy_curr->next = curr->next;
		}
	}
	element_size = rhs.element_size;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
bool operator==(const hashtable<K, V, Ref, Ptr, HF, GK, CK> &lhs, const hashtable<K, V, Ref, Ptr, HF, GK, CK> &rhs)
{//比较哈希表是否相等
	if (lhs.element_size != rhs.element_size ||
		lhs.bucket_count() != rhs.bucket_count()) return false;//首先判断元素个数和桶个数
	for (size_t i = 0; i != lhs.bucket_count(); ++i)
	{//再判定每个桶里的元素是否一样
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
{//不重复插入
	resize(element_size + 1);//判定是否需要重新分配哈希表
	size_t bkt_num = bucket_number(v);
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		if (equal(get_key(curr->value), get_key(v)))//若带插入元素已存在
			return pair<self, bool>(self(curr, this), false);//则插入失败，first成员指向该元素
	node *p_insert_node = new_node(v);
	p_insert_node->next = buckets[bkt_num];
	buckets[bkt_num] = p_insert_node;
	++element_size;
	return pair<self, bool>(self(p_insert_node, this), true);//否则插入，first指向新插入元素
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self hashtable<K, V, Ref, Ptr, HF, GK, CK>::insert_equal(const value_type &v)
{//可重复插入
	resize(element_size + 1);
	size_t bkt_num = bucket_number(v);
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		if (equal(get_key(curr->value), get_key(v)))
		{//找到键相同的元素，若有，则紧挨着插入
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
	return self(p_insert_node, this);//否则在链表头插入
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
size_t hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(const key_type &key)
{//删除键为key的所有元素
	pair<self, self> low_high = equal_range(key);//取得该键范围
	size_t num = 0;
	for (self beg = low_high.first; beg != low_high.second; ++beg)
		++num;
	erase(low_high.first, low_high.second);
	return num;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(self p)
{//删除指定位置的元素
	pair<self, bool> pre = find_pre(p);//查找其前驱
	if (pre.second)
	{//若是某链表首元素
		size_t bkt_num = bucket_number(*p);
		buckets[bkt_num] = p.curr->next;
		delete_node(p.curr);
	}
	else
	{//否则
		pre.first.curr->next = p.curr->next;
		delete_node(p.curr);
	}
	--element_size;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::erase(self first, self last)
{//删除一段元素
	if (first == last) return;
	size_t first_bkt_num = bucket_number(*first), last_bkt_num, num = 0;//first所在桶子编号
	if (last != end()) last_bkt_num = bucket_number(*last);//last所在桶子编号
	else last_bkt_num = buckets.size() - 1;
	//若bool为真，则first所指是该链第一个元素，无前驱，pre的first成员指向first所指元素，否则指向first前驱
	pair<self, bool> pre = find_pre(first);
	for (size_t i = first_bkt_num + 1; i < last_bkt_num; ++i)
	{//先删除first所在桶和last所在桶之间的元素
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
	{//删除first所在桶中first开始的元素，直到遇到last或者删除完毕
		node *p = r;
		r = r->next;
		delete_node(p);
		++num;
	}
	if (pre.second)//根据first是否是该链第一个元素设置桶内的值
		buckets[first_bkt_num] = r;
	else pre.first.curr->next = r;
	if (first_bkt_num != last_bkt_num)
	{//若first和last所在的桶不同，则还要删除last所在链中last之前的所有元素
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
	element_size -= num;//修改元素个数
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::low_bound(const key_type &key)const
{//找到键为key的第一个元素
	size_t bkt_num = bucket_number(key);
	node *curr = buckets[bkt_num];
	while (curr != nullptr && !equal(get_key(curr->value), key))
		curr = curr->next;
	return self(curr, this);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::high_bound(const key_type &key)const
{//找到键位key的最后一个元素，返回其下一位置
	size_t bkt_num = bucket_number(key);
	self low = low_bound(key);
	while (low != end() && equal(get_key(low.curr->value), key))
		++low;
	return low;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
typename hashtable<K, V, Ref, Ptr, HF, GK, CK>::self
hashtable<K, V, Ref, Ptr, HF, GK, CK>::find(const key_type &key)const
{//查找键位key的元素
	size_t bkt_num = bucket_number(key);
	node *curr;
	for (curr = buckets[bkt_num]; curr && !equal(get_key(curr->value), key); curr = curr->next);
	return self(curr, this);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
size_t hashtable<K, V, Ref, Ptr, HF, GK, CK>::count(const key_type &key)const
{//统计键为key的元素的个数
	pair<self, self> low_high = equal_range(key);
	size_t cnt = 0;
	for (self first = low_high.first; first != low_high.second; ++first)
		++cnt;
	return cnt;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::resize(size_t new_element_size)
{//重置哈希表大小
	if (new_element_size > buckets.size())
	{//元素数量超过桶数量即重置
		vector<node*> new_buckets(2 * new_element_size, nullptr);//构建临时vector，为元素个数两倍大
		for (size_t i = 0; i != buckets.size(); ++i)
		{//逐链表复制
			node *curr = buckets[i];
			while (curr != nullptr)
			{//若链不空，不断采用头插法将新元素插入临时vector
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
{//统计某个桶中元素数量
	size_t num = 0;
	for (node *curr = buckets[bkt_num]; curr; curr = curr->next)
		++num;
	return num;
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::swap(hashtable &rhs)
{//交换两个哈希表
	std::swap(hash, rhs.hash);
	std::swap(equal, rhs.equal);
	std::swap(get_key, rhs.get_key);
	std::swap(element_size, rhs.element_size);
	buckets.swap(rhs.buckets);
}

template <class K, class V, class Ref, class Ptr, class HF, class GK, class CK>
void hashtable<K, V, Ref, Ptr, HF, GK, CK>::bucket_print()const
{//打印哈希表，按每个桶打印
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