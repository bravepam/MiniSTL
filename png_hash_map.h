
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

//以hashtable为底层机制实现的

#ifndef PNG_HASH_MAP_H
#define PNG_HASH_MAP_H

#include"png_hashtable.h"
#include<iostream>
#include<utility>

using namespace std;

template <typename Pair>
struct return1st
{//取键器，取得pair类型的键值，函数对象
	typedef typename Pair::first_type first_type;
	const first_type& operator()(const Pair &p)const
	{
		return p.first;
	}
};

template <class Key,class Value,class HashFunction = hash<Key>,class CompareKey = equal_to<Key>>
class png_hash_map
{
private:
	typedef pair<const Key, Value> V;//存储的值类型，是一个键值对
	typedef hashtable<Key, V, V&, V*, HashFunction, return1st<V>, CompareKey> ht;//所用哈希表类型
	friend bool operator==<>(const png_hash_map&, const png_hash_map&);

	ht map;

public://各类型别名
	typedef typename ht::key_type								key_type;
	typedef typename ht::value_type								value_type;
	typedef Value												data_type;
	typedef Value												mapped_type;
	typedef typename ht::hasher									hasher;
	typedef typename ht::key_equal								key_equal;

	typedef typename ht::size_type								size_type;
	typedef typename ht::difference_type						difference_type;
	typedef typename ht::iterator								iterator;
	typedef typename ht::const_iterator							const_iterator;
	typedef typename ht::reference								reference;
	typedef typename ht::const_reference						const_reference;
	typedef typename ht::pointer								pointer;
	typedef typename ht::const_pointer							const_pointer;

	hasher hash_function()const { return map.hash_function(); }
	key_equal get_equal()const { return map.get_equal(); }

public://各构造函数
	png_hash_map() :map(60, hasher(), key_equal()){}
	explicit png_hash_map(size_t n) :map(n, hasher(), key_equal()){}
	png_hash_map(size_t n, const hasher &h) :map(n, h, key_equal()){}
	png_hash_map(size_t n, const hasher &h, const key_equal &ke) :map(n, h, ke){}
	png_hash_map(const png_hash_map &rhs) :map(rhs.map){}
	png_hash_map& operator=(png_hash_map &rhs)
	{
		if (&rhs != this)
			map = rhs.map;
		return *this;
	}
	template<typename InputIterator>
	png_hash_map(InputIterator first, InputIterator last) : png_hash_map()
	{
		map.insert_unique(first, last); 
	}
	template <typename InputIterator>
	png_hash_map(InputIterator first, InputIterator last, size_t n) : png_hash_map(n)
	{
		map.insert_unique(first, last);
	}
	template <typename InputIterator>
	png_hash_map(InputIterator first, InputIterator last, size_t n, const hasher &h) : png_hash_map(n, h)
	{
		map.insert_unique(first, last);
	}
	template <typename InputIterator>
	png_hash_map(InputIterator first, InputIterator last, size_t n, const hasher &h, const key_equal &ke)
		: png_hash_map(n, h, ke){
		map.insert_unique(first, last);
	}

public://全部是直接调用hashtable的接口
	size_t size()const { return map.size(); }
	bool empty()const { return map.empty(); }
	void swap(png_hash_map &rhs){ map.swap(rhs.map); }
	iterator begin() { return map.begin(); }
	iterator end() { return map.end(); }
	const_iterator begin()const { return map.begin(); }
	const_iterator end()const { return map.end(); }
	pair<iterator, bool> insert(const value_type &v)
	{
		return map.insert_unique(v);
	}
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		map.insert_unique(first, last);
	}
	iterator find(const key_type &key)
	{
		return map.find(key);
	}
	const_iterator find(const key_type &key)const
	{
		return map.find(key);
	}
	data_type& operator[](const key_type &key)
	{
		return (*((map.insert_unique(pair<key_type, data_type>(key, data_type()))).first)).second;
	}
	size_t count(const key_type &key) const { return map.count(key); }
	pair<iterator, iterator> equal_range(const key_type &key)
	{
		return map.equal_range(key);
	}
	pair<const_iterator, const_iterator> equal_range(const key_type &key)const
	{
		return map.equal_range(key);
	}
	size_t erase(const key_type &key){ return map.erase(key); }
	void erase(iterator p) { map.erase(p); }
	void erase(iterator first, iterator last) { map.erase(first, last); }
	void clear(){ map.clear(); }

	void resize(size_t n) { map.resize(n); }
	size_t bucket_count()const { return map.bucket_count(); }
	size_t element_in_bucket(size_t i)const { return map.element_in_bucket(i); }
};

template <class K,class V,class HF,class CK>
bool operator==(const png_hash_map<K, V, HF, CK> &lhs, const png_hash_map<K, V, HF, CK> &rhs)
{
	return lhs.map == rhs.map;
}

#endif