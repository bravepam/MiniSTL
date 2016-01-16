
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
{//������������֮��ľ���
	size_t len = 0;
	for (; b != e; ++b)
		++len;
	return len;
}

template <typename T>
class png_vector
{
public:
	//���ͱ���
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

	iterator start;//�ڴ�ʹ����ʼ��
	iterator last;//�ڴ�ʹ��ĩ����һλ��
	iterator end_of_storage;//������ĩ����һλ��
	static allocator<T> alloc;//�ڴ����������
	//���ܺ���
	iterator allocate_fill(size_type, const T&);//�����ڴ�ͬʱ�������
	void initialized(size_type size, const T &value)
	{//��ʼ��
		start = allocate_fill(size, value);
		last = start + size;
		end_of_storage = last;
	}
	template <typename InputIterator>
	void initialized(InputIterator b, InputIterator e)
	{//��ʼ��������
		size_type size = length(b,e);
		start = alloc.allocate(size);
		uninitialized_copy(b, e, start);
		last = start + size;
		end_of_storage = last;
	}
	void destroy(iterator beg, iterator end)
	{//���ٶ��󣨵�����������������������������
		for (iterator p = beg; p != end; p++)
			alloc.destroy(p);
	}
public:
	//���캯��
	png_vector() :start(0), last(0), end_of_storage(0){};//Ĭ�Ϲ��캯��
	template <typename InputIterator>
	png_vector(InputIterator b, InputIterator e) { initialized(b, e); }
	png_vector(const png_vector &vec) { initialized(vec.begin(), vec.end()); }//���ƹ��캯��
	png_vector(size_type size, const T &value) { initialized(size, value); }
	png_vector(int size, const T &value) { initialized(size, value); }
	png_vector(long size, const T &value) { initialized(size, value); }
	explicit png_vector(size_type size) { initialized(size, T()); }
	//��ֵ����
	png_vector& operator=(const png_vector&);
	void swap(png_vector&);//������������
	template <typename InputIterator>
	void assign(InputIterator beg, InputIterator end);//��[beg,end)��ΧԪ����������
	void assign(size_type, const T&);
	//�йش�С�Ĳ���
	size_type size()const{ return (last - start); }
	size_type capacity()const { return (end_of_storage - start); }//����������
	bool empty()const { return start == last; }//�п�
	void resize(size_type, const T& = T());//����������С
	//begin��end����
	iterator begin()const { return start; }
	iterator end()const { return last; }
	//��������Ԫ�ز���
	reference back() { return *(end() - 1); }
	const_reference back()const { return *(end() - 1); }
	reference front() { return *begin(); }
	const_reference front()const { return *begin(); }
	reference operator[](size_type index) { return *(begin() + index); }
	const_reference operator[](size_type index)const { return *(begin() + index); }
	reference at(size_type index)
	{//�൱��[]����������ȫ
		if (static_cast<size_type>(last - start) > index) return *(begin() + index);
		else
		{
			printf("warning!out of storage,access deny! at(%d)\n", index);
			exit(-1);
		}
	}
	const_reference at(size_type index)const
	{//�൱��[]����������ȫ
		if (static_cast<size_type>(last - start) > index) return *(begin() + index);
		else
		{
			printf("warning!out of storage,access deny! at(%d)\n", index);
			exit(-1);
		}
	}
	//���Ԫ�ز���
	void push_back(const T &value)
	{
		if (last != end_of_storage)
		{
			alloc.construct(last, value);
			++last;
		}
		else insert(end(), 1, value);
	}
	iterator insert(iterator p, const T &value) { return insert(p, 1, value); }//����һ��Ԫ��
	iterator insert(iterator, size_type, const T&);//������
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);//����һ��Ԫ��
	//ɾ��Ԫ�ز���
	iterator erase(iterator p)
	{//ɾ��һ��Ԫ��
		if ((p + 1) != last)
			copy(p + 1, last, p);
		--last;
		alloc.destroy(last);
		return p;
	}
	iterator erase(iterator beg, iterator end)
	{//ɾ��һ��Ԫ��[beg,end)
		if (end != last)
			copy(end, last, beg);
		destroy((last - (end - beg)), last);
		last = last - (end - beg);
		return beg;
	}
	void clear()
	{//�������
		erase(begin(), end());
	}
	void pop_back()
	{//ɾ��β��Ԫ��
		--last;
		alloc.destroy(last);
	}
	//��������
	~png_vector()
	{
		destroy(start, last);
		if (start != NULL)
			alloc.deallocate(start, end_of_storage - start);//�ͷſռ�
	}
};

template <typename T> allocator<T> png_vector<T>::alloc;//����alloc

template <typename T>
typename png_vector<T>::iterator png_vector<T>::allocate_fill(size_type size, const T &value)
{//�����ڴ�ռ䣬���������
	iterator r = alloc.allocate(size);
	uninitialized_fill_n(r, size, value);
	return r;
}

template <typename T>
png_vector<T>& png_vector<T>::operator=(const png_vector &vec)
{//��ֵ������
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
	{//�����sizeС��ԭsize����ֱ�����ٶ������ݶ���
		destroy(start + new_size, last);
		last = start + new_size;
	}
	else//�����ڣ�����β�˲�����������
		insert(last, new_size - old_size, value);
}

template <typename T>
typename png_vector<T>::iterator
png_vector<T>::insert(iterator p, size_type size, const T &value)
{
	if (size > 0)
	{//size����0ʱ�Ų���Ԫ��
		if (static_cast<size_type>(end_of_storage - last) >= size)
		{//��ʣ�������㹻
			copy_backward(p, last, last + size);//�ֽ�������Ԫ�غ���size����λ
			uninitialized_fill_n(p, size, value);//�ٲ���������
			last += size;
			if (size == 1) return p;//ֻ����һ��Ԫ��ʱ�����ز�����������ֻΪ�����׼һ��
			return 0;
		}
		else
		{//�����������������´������������
			size_type old_size = this->size();//��þ�������С
			size_type new_size = old_size + (size > old_size ? size : old_size);//���㼴�������ڴ��С
			iterator new_start = alloc.allocate(new_size);
			iterator new_last = new_start, old_start = start;
			new_last = uninitialized_copy(start, p, new_last);//��ԭ���������֮ǰ��Ԫ�ظ��Ƶ�������
			uninitialized_fill_n(new_last, size, value);//������������Ԫ��
			new_last += size;//ͬ������new_last
			new_last = uninitialized_copy(p, last, new_last);//��ԭ���������֮���Ԫ�ظ��Ƶ�������
			//�ͷ�ԭ�����ռ�
			this->~png_vector();
			//���µ�����
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
{//������insert���ƣ�ֻ�ǲ������һ��Ԫ�أ�����Ԫ�ز�ͬ
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
		//�ͷ�ԭ�����ռ�
		this->~png_vector();
		//���µ�����
		start = new_start;
		last = new_last;
		end_of_storage = new_start + new_size;
	}
}

template <typename T>
void swap_iterator(T &iter1, T &iter2)
{//��������ֵ����png_vector���Ա����
	T iter = iter1;
	iter1 = iter2;
	iter2 = iter;
}

template <typename T>
void png_vector<T>::swap(png_vector &vec)
{//������������
	swap_iterator(start, vec.start);
	swap_iterator(last, vec.last);
	swap_iterator(end_of_storage, vec.end_of_storage);
}

template <typename T> template <typename InputIterator>
void png_vector<T>::assign(InputIterator beg, InputIterator end)
{//��������
	//if( (beg >= end_of_storage) || (end <= start))
	//{//������ָ��ͬһ�����ĵ�����
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