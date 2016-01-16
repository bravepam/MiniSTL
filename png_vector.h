
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_VECTOR_H
#define PNG_VECTOR_H

#define _SCL_SECURE_NO_WARNINGS

#include<iostream>
#include<memory>

using namespace std;
#define max(i,j) i > j ? i : j

template <typename InputIterator>
static size_t length(InputIterator b, InputIterator e)
{//求两个迭代器之间的距离
	size_t len = 0;
	for (; b != e; ++b)
		++len;
	return len;
}

template <typename T>
class png_vector
{
public:
	//类型别名
	typedef size_t											size_type;
	typedef T												value_type;
	typedef value_type*										iterator;
	typedef const T*										const_iterator;
	typedef ptrdiff_t										difference_type;
	typedef T&												reference;
	typedef const T&										const_reference;
private:
	friend bool operator< <T>(const png_vector<T>&, const png_vector<T>&);
	friend bool operator== <T>(const png_vector<T>&, const png_vector<T>&);

	iterator start;//内存使用起始点
	iterator last;//内存使用末端下一位置
	iterator end_of_storage;//总容量末端下一位置
	static allocator<T> alloc;//内存分配器对象
	//功能函数
	iterator allocate_fill(size_type, const T&);//分配内存同时填充数据
	void initialized(size_type size, const T &value)
	{//初始化
		start = allocate_fill(size, value);
		last = start + size;
		end_of_storage = last;
	}
	template <typename InputIterator>
	void initialized(InputIterator b, InputIterator e)
	{//初始化，重载
		size_type size = length(b,e);
		start = alloc.allocate(size);
		uninitialized_copy(b, e, start);
		last = start + size;
		end_of_storage = last;
	}
	void destroy(iterator beg, iterator end)
	{//销毁对象（调用析构函数），接受两个迭代器
		for (iterator p = beg; p != end; p++)
			alloc.destroy(p);
	}
public:
	//构造函数
	png_vector() :start(0), last(0), end_of_storage(0){};//默认构造函数
	template <typename InputIterator>
	png_vector(InputIterator b, InputIterator e) { initialized(b, e); }
	png_vector(const png_vector &vec) { initialized(vec.begin(), vec.end()); }//复制构造函数
	png_vector(size_type size, const T &value) { initialized(size, value); }
	png_vector(int size, const T &value) { initialized(size, value); }
	png_vector(long size, const T &value) { initialized(size, value); }
	explicit png_vector(size_type size) { initialized(size, T()); }
	//赋值操作
	png_vector& operator=(const png_vector&);
	void swap(png_vector&);//交换两个容器
	template <typename InputIterator>
	void assign(InputIterator beg, InputIterator end);//用[beg,end)范围元素重置容器
	void assign(size_type, const T&);
	//有关大小的操作
	size_type size()const{ return (last - start); }
	size_type capacity()const { return (end_of_storage - start); }//容器总容量
	bool empty()const { return start == last; }//判空
	void resize(size_type, const T& = T());//重置容器大小
	//begin和end操作
	iterator begin()const { return start; }
	iterator end()const { return last; }
	//访问容器元素操作
	reference back() { return *(end() - 1); }
	const_reference back()const { return *(end() - 1); }
	reference front() { return *begin(); }
	const_reference front()const { return *begin(); }
	reference operator[](size_type index) { return *(begin() + index); }
	const_reference operator[](size_type index)const { return *(begin() + index); }
	reference at(size_type index)
	{//相当于[]，不过更安全
		if (static_cast<size_type>(last - start) > index) return *(begin() + index);
		else
		{
			printf("warning!out of storage,access deny! at(%d)\n", index);
			exit(-1);
		}
	}
	const_reference at(size_type index)const
	{//相当于[]，不过更安全
		if (static_cast<size_type>(last - start) > index) return *(begin() + index);
		else
		{
			printf("warning!out of storage,access deny! at(%d)\n", index);
			exit(-1);
		}
	}
	//添加元素操作
	void push_back(const T &value)
	{
		if (last != end_of_storage)
		{
			alloc.construct(last, value);
			++last;
		}
		else insert(end(), 1, value);
	}
	iterator insert(iterator p, const T &value) { return insert(p, 1, value); }//插入一个元素
	iterator insert(iterator, size_type, const T&);//插入多个
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);//插入一段元素
	//删除元素操作
	iterator erase(iterator p)
	{//删除一个元素
		if ((p + 1) != last)
			copy(p + 1, last, p);
		--last;
		alloc.destroy(last);
		return p;
	}
	iterator erase(iterator beg, iterator end)
	{//删除一段元素[beg,end)
		if (end != last)
			copy(end, last, beg);
		destroy((last - (end - beg)), last);
		last = last - (end - beg);
		return beg;
	}
	void clear()
	{//清空容器
		erase(begin(), end());
	}
	void pop_back()
	{//删除尾部元素
		--last;
		alloc.destroy(last);
	}
	//析构函数
	~png_vector()
	{
		destroy(start, last);
		if (start != NULL)
			alloc.deallocate(start, end_of_storage - start);//释放空间
	}
};

template <typename T> allocator<T> png_vector<T>::alloc;//定义alloc

template <typename T>
typename png_vector<T>::iterator png_vector<T>::allocate_fill(size_type size, const T &value)
{//分配内存空间，并填充数据
	iterator r = alloc.allocate(size);
	uninitialized_fill_n(r, size, value);
	return r;
}

template <typename T>
png_vector<T>& png_vector<T>::operator=(const png_vector &vec)
{//赋值操作符
	if (this != &vec)
	{
		clear();
		insert(start, vec.begin(), vec.end());
	}
	return *this;
}

template <typename T>
void png_vector<T>::resize(size_type new_size, const T &value = T())
{
	size_type old_size = size();
	if (new_size < old_size)
	{//如果新size小于原size，则直接销毁多余数据对象
		destroy(start + new_size, last);
		last = start + new_size;
	}
	else//若大于，则在尾端插入多余的数据
		insert(last, new_size - old_size, value);
}

template <typename T>
typename png_vector<T>::iterator
png_vector<T>::insert(iterator p, size_type size, const T &value)
{
	if (size > 0)
	{//size大于0时才插入元素
		if (static_cast<size_type>(end_of_storage - last) >= size)
		{//若剩余容量足够
			copy_backward(p, last, last + size);//现将插入点后元素后移size个单位
			uninitialized_fill_n(p, size, value);//再插入新数据
			last += size;
			if (size == 1) return p;//只插入一个元素时，返回插入点迭代器，只为了与标准一致
			return 0;
		}
		else
		{//若容量不够，则重新创建更大的容器
			size_type old_size = this->size();//获得旧容器大小
			size_type new_size = old_size + (size > old_size ? size : old_size);//计算即将分配内存大小
			iterator new_start = alloc.allocate(new_size);
			iterator new_last = new_start, old_start = start;
			new_last = uninitialized_copy(start, p, new_last);//将原容器插入点之前的元素复制到新容器
			uninitialized_fill_n(new_last, size, value);//在新容器插入元素
			new_last += size;//同步更新new_last
			new_last = uninitialized_copy(p, last, new_last);//将原容器插入点之后的元素复制到新容器
			//释放原容器空间
			this->~png_vector();
			//更新迭代器
			start = new_start;
			last = new_last;
			end_of_storage = new_start + new_size;
			if (size == 1) return start + (p - old_start);
			return 0;
		}
	}
	return 0;
}

template <typename T> template <typename InputIterator>
void png_vector<T>::insert(iterator p, InputIterator beg, InputIterator end)
{//与上述insert类似，只是插入的是一段元素，各个元素不同
	size_type size = length(beg,end);
	if (size <= 0) return;
	if (static_cast<size_type>(end_of_storage - last) >= size)
	{
		copy_backward(p, last, last + size);
		uninitialized_copy(beg, end, p);
		last += size;
	}
	else
	{
		size_type old_size = this->size();
		size_type new_size = old_size + max(old_size, size);
		iterator new_start = alloc.allocate(new_size);
		iterator new_last = new_start;
		new_last = uninitialized_copy(start, p, new_last);
		new_last = uninitialized_copy(beg, end, new_last);
		new_last = uninitialized_copy(p, last, new_last);
		//释放原容器空间
		this->~png_vector();
		//更新迭代器
		start = new_start;
		last = new_last;
		end_of_storage = new_start + new_size;
	}
}

template <typename T>
void swap_iterator(T &iter1, T &iter2)
{//交换两个值，非png_vector类成员函数
	T iter = iter1;
	iter1 = iter2;
	iter2 = iter;
}

template <typename T>
void png_vector<T>::swap(png_vector &vec)
{//交换两个容器
	swap_iterator(start, vec.start);
	swap_iterator(last, vec.last);
	swap_iterator(end_of_storage, vec.end_of_storage);
}

template <typename T> template <typename InputIterator>
void png_vector<T>::assign(InputIterator beg, InputIterator end)
{//重置容器
	//if( (beg >= end_of_storage) || (end <= start))
	//{//若不是指向同一容器的迭代器
	//	clear();
	//	insert(start,beg,end);
	//}
	//else
	//{
	//	cout << "warning!iterator refers to the same vector" << endl;
	//	return;
	//}
	clear();
	insert(start, beg, end);
}

template <typename T>
void png_vector<T>::assign(size_type size, const T &value)
{
	clear();
	insert(start, size, value);
}

template <typename T> bool operator<(const png_vector<T> &lhs, const png_vector<T> &rhs)
{
	if (lhs.size() < rhs.size())
		return true;
	else if (lhs.size() > rhs.size())
		return false;
	else
	{
		if (lhs.size() == 0) return false;
		for (png_vector<T>::size_type i = 0; i != lhs.size(); ++i)
			if (lhs[i] >= rhs[i])
				return false;
		return true;
	}
}

template <typename T> bool operator==(const png_vector<T> &lhs, const png_vector<T> &rhs)
{
	if (lhs.size() != rhs.size())
		return false;
	for (png_vector<T>::size_type i = 0; i != lhs.size(); ++i)
		if (lhs[i] != rhs[i])
			return false;
	return true;
}

#endif