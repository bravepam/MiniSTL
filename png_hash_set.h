
/*******************************************
 * Author: bravepam
 *
 * E-mail:1372120340@qq.com
 *******************************************
 */

#ifndef PNG_HASH_SET_H
#define PNG_HASH_SET_H

//png_hash_set，采用hash表作为底层机制

#include"png_hashtable.h"//同目录下，E:\Study\projects\BaiduYun\program\PNG_STL
#include<iostream>

using namespace std;

template < class Key, class HashFunction = hash<Key>,
class GetKey = identity<Key>, class CompareKey = equal_to<Key> >
class png_hash_set
{
private:
	friend bool operator==<>(const png_hash_set&, const png_hash_set&);
	//采用的哈希表类型
	typedef hashtable<Key, Key, const Key&,const Key*,HashFunction, GetKey, CompareKey> ht;
	ht set;

public://各类型别名
	typedef typename ht::size_type							size_type;
	typedef typename ht::difference_type					difference_type;
	typedef typename ht::key_type							key_type;
	typedef typename ht::value_type							value_type;
	typedef typename ht::const_iterator						iterator;//容器中值不可改变，
	typedef typename ht::const_iterator						const_iterator;//故均为const类型
	typedef typename ht::const_reference					reference;
	typedef typename ht::const_reference					const_reference;
	typedef typename ht::hasher								hasher;
	typedef typename ht::key_equal							key_equal;
	hasher hash_function()const { return set.hash_function(); }
	key_equal  get_equal()const { return set.get_equal(); }

public://诸多构造函数
	png_hash_set() :set(60, hasher(), key_equal()){}
	explicit png_hash_set(size_t n) :set(n, hasher(), key_equal()){}
	png_hash_set(size_t n, const hasher &hf) :set(n, hf, key_equal()){}
	png_hash_set(size_t n, const hasher &hf, const key_equal &ke) :set(n, hf, ke){}
	png_hash_set(const png_hash_set &rhs) :set(rhs.set){}
	png_hash_set& operator=(png_hash_set &rhs)
	{
		if (&rhs != this)
			set = rhs.set;
		return *this;
	}
	template <class InputIterator>
	png_hash_set(InputIterator first, InputIterator last) : png_hash_set()
	{
		set.insert_unique(first, last);//不可重复插入，下同
	}
	template<class InputIterator>
	png_hash_set(InputIterator first, InputIterator last, size_t n) : png_hash_set(n)
	{
		set.insert_unique(first, last);
	}
	template <typename InputIterator>
	png_hash_set(InputIterator first, InputIterator last, size_t n, const hasher &hf) : png_hash_set(n, hf)
	{
		set.inert_unique(first, last);
	}
	template <typename InputIterator>
	png_hash_set(InputIterator first, InputIterator last, size_t n, const hasher &hf, const key_equal &ke) :
		png_hash_set(n, hf, ke)
	{
		set.insert_unique(first, last);
	}

public://各功能函数，均直接调用哈希表接口
	size_t size()const{ return set.size(); }
	size_t bucket_count()const { return set.bucket_count(); }
	bool empty()const { return set.empty(); }

	void swap(png_hash_set &rhs){ set.swap(rhs.set); }
	iterator begin()const { return set.begin(); }
	iterator end()const { return set.end(); }

	pair<iterator, bool> insert(const value_type &v)
	{
		return set.insert_unique(v);
	}
	template <typename InputIterator>
	void insert(InputIterator first, InputIterator last)
	{
		set.insert_unique(first, last);
	}
	size_t erase(const key_type &key)
	{
		return set.erase(key);
	}
	void erase(iterator p)
	{
		set.erase(p);
	}
	void erase(iterator first, iterator last)
	{
		set.erase(first, last);
	}
	iterator find(const key_type &key)const { return set.find(key); }
	size_t count(const key_type &key)const { return set.count(key); }
	pair<iterator, iterator> equal_range(const key_type &key)const { return set.equal_range(key); }
	void clear(){ set.clear(); }
	void resize(size_t n){ set.resize(n); }
	size_t element_in_bucket(size_t i)const { return set.element_in_bucket(i); }
};

template <class K,class HF,class GK,class CK>
bool operator==(const png_hash_set<K, HF, GK, CK> &lhs, const png_hash_set<K, HF, GK, CK> &rhs)
{
	return lhs.set == rhs.set;
}

#endif