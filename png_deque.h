
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_DEQUE_H
#define PNG_DEQUE_H

#define _SCL_SECURE_NO_WARNINGS

#include<iostream>
//#include<memory>

using namespace std;

template <typename InputIterator>
static size_t length(InputIterator b, InputIterator e)
{
	size_t len = 0;
	for (; b != e; ++b)
		++len;
	return len;
}

template <typename T, size_t buffersize> class png_deque;

template <typename T, size_t buffersize>
class deque_iterator
{
public:
	//�����ͱ���
	typedef random_access_iterator_tag							iterator_category;//����������
	typedef deque_iterator<T, buffersize>						iterator;
	typedef T*													pointer;
	typedef const T*											const_pointer;
	typedef T													value_type;
	typedef T&													reference;
	typedef const T&											const_reference;
	typedef size_t												size_type;
	typedef ptrdiff_t											difference_type;
	typedef T**													map_pointer;
private:
	//deque�ĵ�������װ���ĸ�ָ��
	friend class png_deque < T, buffersize>;
	T *first;
	T *last;
	T *current;
	map_pointer ptr_map;
public:
	deque_iterator() :first(NULL), last(NULL), current(NULL), ptr_map(NULL){}
	deque_iterator(pointer ptr) :current(ptr), first(NULL), last(NULL), ptr_map(NULL){}//û�����
	void set_new_node(map_pointer new_node)
	{//���õ�������ָ�Ļ�����
		ptr_map = new_node;
		first = *ptr_map;
		last = first + static_cast<difference_type>(buffersize);
	}
	//�������������
	reference operator*() { return *current; }
	const_reference operator*() const { return *current; }
	pointer operator->() { return &(*current); }
	const_pointer operator->() const { return &(*current); }
	difference_type operator-(const iterator &rhs) const
	{
		return static_cast<difference_type>((ptr_map - rhs.ptr_map - 1) * buffersize
			+ (current - first) + (rhs.last - rhs.current));
	}
	iterator& operator++()
	{
		++current;
		if (current == last)
		{
			set_new_node(ptr_map + 1);
			current = first;
		}
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
		if (current == first)
		{
			set_new_node(ptr_map - 1);
			current = last;
		}
		--current;
		return *this;
	}
	iterator operator--(int)
	{
		iterator tmp = *this;
		--*this;
		return tmp;
	}
	bool operator==(const iterator &rhs)const
	{
		return current == rhs.current;
	}
	bool operator!=(const iterator &rhs)const
	{
		return current != rhs.current;
	}
	bool operator<(const iterator &rhs)const
	{//ֻ������С�ڡ�С�ڻ����
		return (ptr_map == rhs.ptr_map) ? (current < rhs.current) : (ptr_map < rhs.ptr_map);
	}
	bool operator<=(const iterator &rhs)const
	{
		return (*this < rhs) || (*this == rhs);
	}
	iterator& operator+=(difference_type n)
	{//����������������мӻ���Ļ�����ע��n�����ɸ�
		difference_type offset = (current - first) + n;
		difference_type map_offset = offset >= 0 ? (offset / buffersize) :
			(-static_cast<difference_type>((-offset - 1) / buffersize) - 1);//offsetΪ��ʱ��mapƫ�����ļ���
		if (map_offset != 0)//��Ϊ0ʱ˵����ָ�����������ı䣬��Ҫ����
			set_new_node(ptr_map + map_offset);
		current = first + (offset - map_offset * buffersize);
		return *this;
	}
	iterator operator+(difference_type n)
	{
		iterator tmp = *this;
		tmp += n;
		return tmp;
	}
	iterator& operator-=(difference_type n)
	{
		*this += -n;
		return *this;
	}
	iterator operator-(difference_type n)
	{
		iterator tmp = *this;
		tmp -= n;
		return tmp;
	}
};

template <typename T, size_t buffersize = 10>//Ĭ�ϻ�������СΪ10
class png_deque
{
public:
	//�����ͱ���
	typedef T													          value_type;
	typedef size_t												          size_type;
	typedef ptrdiff_t											          difference_type;
	typedef typename deque_iterator<T, buffersize>::iterator			  iterator;
	typedef T&													          reference;
	typedef const T&											          const_reference;
private:
	typedef T**													map_pointer;
	typedef value_type*											pointer;
	iterator start;//ָ���һ�������������з�װ��currentָ���һ��Ԫ��
	iterator finish;//ָ�����һ�������������з�װ��currentָ����ĩԪ�ص���һλ��
	map_pointer map;//ָ����ν�п������洢�и���������ַ
	size_type mapsize;//�п�����С
	static allocator<T> data_alloc;//Ԫ���ڴ������
	static allocator<pointer> map_alloc;//�п����ڴ������
	size_type get_mapsize()
	{//���map��С�����map��СΪ8.
		if (mapsize <= 0)
			mapsize = 8;
		return mapsize;
	}
	//����������������Ҫ���������������������ķ���ͳ�ʼ��
	void fill_initialized(size_type, const T&);
	template <typename InputIterator>
	void fill_initialized(InputIterator, InputIterator);
	void alloc_map_and_buff(size_type);
	//���º������𻺳�����ʱ������Ĳ��빤��
	void push_back_aux(const T &);
	void full_back(size_type need_add_map = 1)
	{//�ж��п���β���Ƿ����㹻ʣ��ռ�
		if (finish.ptr_map - map + need_add_map >= mapsize)
			reallocate_map(need_add_map, true);//���������
	}
	void push_front_aux(const T&);
	void full_front(size_type need_add_map = 1)
	{//�ж��п���ǰ���Ƿ���ʣ��ռ�
		if (start.ptr_map - need_add_map < map)
			reallocate_map(need_add_map, false);
	}
	void reallocate_map(size_type need_add_map, bool full_at_back);
	//����p��ָ��n������
	void destroy(iterator p, size_type n)
	{
		for (size_t i = 0; i != n; ++i)
			data_alloc.destroy(p.current + i);
	}
public:
	//���ֹ��캯��������Ŀ�����Ĭ����һ��������
	png_deque() :start(), finish(), map(NULL), mapsize(0){ alloc_map_and_buff(0); }
	png_deque(const png_deque &rhs) { fill_initialized(rhs.begin(), rhs.end()); }
	template <typename InputIterator>
	png_deque(InputIterator b, InputIterator e) { fill_initialized(b, e); }
	png_deque(size_type n, const T &value = T()) { fill_initialized(n, value); }
	png_deque(int n, const T &value = T()) { fill_initialized(n, value); }
	png_deque(long n, const T &value = T()) { fill_initialized(n, value); }
	//begin��end����
	iterator begin()const { return start; }
	iterator end()const { return finish; }
	//���Ԫ��
	void push_back(const T&);
	void push_front(const T&);
	iterator insert(iterator, const T&);
	void insert(iterator, size_type, const T&);
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);
	//������С����
	size_type size()const { return finish - start; }
	bool empty()const { return start == finish; }
	void resize(size_type, const T & = T());
	//��������Ԫ�ز���
	reference back()const { return *(--end()); }
	reference front()const { return *(begin()); }
	reference operator[](size_type n) { return *(start + n); }
	reference at(size_type n)/*const*/
	{
		if (finish <= (start + n))
			cout << endl << "out of range of deque!" << endl;
		return (*this)[n];
	}
	//ɾ��Ԫ��
	iterator erase(iterator);
	iterator erase(iterator, iterator);
	void clear();
	void pop_back();
	void pop_front();
	//��ֵ����
	png_deque& operator=(const png_deque&);
	void assign(size_type, const T & = T());
	template <typename InputIterator>
	void assign(InputIterator, InputIterator);
	//��������
	~png_deque()
	{
		clear();
		data_alloc.deallocate(start.first, buffersize);
		map_alloc.deallocate(map, mapsize);
	}
};

//���������ڴ������
template <typename T, size_t buffersize> allocator<T> png_deque<T, buffersize>::data_alloc;
template <typename T, size_t buffersize> allocator<typename png_deque<T, buffersize>::pointer>
	png_deque<T, buffersize>::map_alloc;

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::alloc_map_and_buff(size_type element_size)
{//����������п�������ͻ���������
	size_t need_map = element_size / buffersize + 1;//���軺��������
	mapsize = (get_mapsize() > (need_map + 2)) ? get_mapsize() : need_map + 2;
	map = map_alloc.allocate(mapsize);
	map_pointer map_start = map + (mapsize - need_map) / 2;//�����п�����ʼʹ��λ��
	map_pointer map_finish = map_start + need_map - 1;
	for (map_pointer curr = map_start; curr <= map_finish; ++curr)
		*curr = data_alloc.allocate(buffersize);//���仺����,����ַ����map
	//��������������
	start.set_new_node(map_start);
	finish.set_new_node(map_finish);
	start.current = start.first;
	finish.current = finish.first + element_size % buffersize;
}

template < typename T, size_t buffersize>
inline void png_deque<T, buffersize>::fill_initialized(size_type element_size, const T &value)
{
	alloc_map_and_buff(element_size);
	for (map_pointer curr = start.ptr_map; curr < finish.ptr_map; ++curr)
		uninitialized_fill(*curr, *curr + buffersize, value);//��value��ʼ��������
	uninitialized_fill(finish.first, finish.current, value);//�������ʣ��ļ����������������һ�������Ļ�����
}

template <typename T, size_t buffersize> template <typename InputIterator>
inline void png_deque<T, buffersize>::fill_initialized(InputIterator beg, InputIterator end)
{
	size_t element_size = length(beg,end);
	alloc_map_and_buff(element_size);
	InputIterator beg__ = beg, end__;
	if (element_size >= buffersize)
	{
		end__ = beg + buffersize;
		for (map_pointer curr = start.ptr_map; curr < finish.ptr_map; ++curr)
		{//��[beg,end)���Ԫ�س�ʼ��������
			uninitialized_copy(beg__, end__, *curr);
			beg__ = end__;
			end__ += buffersize;
		}
	}
	uninitialized_copy(beg__, end, finish.first);//ͬ��ע��������beg__������end__
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_back(const T &value)
{//�����ĩ�˲���Ԫ��
	if ((finish.current + 1) != finish.last)
	{//����������ռ��㹻��ֱ�Ӳ���
		data_alloc.construct(finish.current, value);
		++finish.current;
	}
	else//���򽻸���
		push_back_aux(value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_back_aux(const T &value)
{
	full_back();//�ж��п����Ƿ��пռ䣬û�������
	data_alloc.construct(finish.current, value);
	//�����������һ�������������µ���finish������
	*(finish.ptr_map + 1) = data_alloc.allocate(buffersize);
	finish.set_new_node(finish.ptr_map + 1);
	finish.current = finish.first;
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_front(const T &value)
{
	if (start.current != start.first)
	{//�ռ��㹻����ֱ�Ӳ���
		--start.current;
		data_alloc.construct(start.current, value);
	}
	else//���򽻸���
		push_front_aux(value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_front_aux(const T &value)
{//����push_back_aux
	full_front();
	*(start.ptr_map - 1) = data_alloc.allocate(buffersize);
	start.set_new_node(start.ptr_map - 1);
	start.current = start.last - 1;
	data_alloc.construct(start.current, value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::reallocate_map(size_type need_add_map, bool full_at_back)
{//���п�����˻���ǰ�˿ռ䲻��
	size_t used_map = finish.ptr_map - start.ptr_map + 1;//�п�����ʹ�ô�С
	size_t new_used_map = used_map + need_add_map;//����ʹ�õ��ܴ�С
	map_pointer new_start_map;
	if (mapsize > 2 * new_used_map)
	{//�п����ִ�ռ仹�㹻
		new_start_map = map + (mapsize - new_used_map) / 2
			+ (full_at_back ? 0 : need_add_map);//���µ����п���ʹ����ʼ��
		if (full_at_back)//�������
			copy(start.ptr_map, finish.ptr_map + 1, new_start_map);
		else//��ǰ����
			copy_backward(start.ptr_map, finish.ptr_map + 1, new_start_map + used_map);
	}
	else
	{//�ִ�ռ䲻���������·��������п���
		size_t new_mapsize = mapsize + (mapsize > need_add_map ? mapsize : need_add_map) + 2;//�����´�С
		map_pointer new_map = map_alloc.allocate(new_mapsize);
		new_start_map = new_map + (new_mapsize - new_used_map) / 2 + (full_at_back ? 0 : need_add_map);//����ʹ����ʼ��
		copy(start.ptr_map, finish.ptr_map + 1, new_start_map);
		map_alloc.deallocate(map, mapsize);//�ͷ�ԭ�����п���
		//��������
		map = new_map;
		mapsize = new_mapsize;
	}
	start.set_new_node(new_start_map);
	finish.set_new_node(start.ptr_map + used_map - 1);
}

template <typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::insert(iterator p, const T &value)
{//����λ�ò���һ��Ԫ��
	if (p == start)
	{//������ʼ��
		push_front(value);
		return start;
	}
	else if (p == finish)
	{//������β��
		push_back(value);
		return (finish - 1);
	}
	else
	{//����
		size_t element_before = p - start;//��������ǰԪ�ظ���
		if (element_before > size() / 2)
		{//��ǰ��Ԫ�ؽ϶࣬������ƶ����Ԫ��
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			copy_backward(p, back2, back1);
		}
		else
		{//������ǰ�ƶ�ǰ��Ԫ��
			push_front(front());
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			copy(front2, p, front1);
			--p;
		}
		*p = value;//����Ԫ��
		return p;
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::insert(iterator p, size_type n, const T &value)
{//����λ�ò���n��Ԫ��value
	size_t deque_size = size(), element_before = p - start;
	if (element_before > deque_size / 2)
	{//ǰ��Ԫ�ؽ϶࣬������ƶ�Ԫ��
		size_t back_remain_size = finish.last - finish.current;//finish��ָ��������ʣ�ռ����
		size_t need_map = (n - back_remain_size) / buffersize + 1;//��Ҫ�ٷ���Ļ���������
		full_back(need_map);//�ж��п�����˿ռ��Ƿ��㹻
		p = start + element_before;//�ؼ���ͬ��
		map_pointer finish_map_ptr = finish.ptr_map;
		for (size_t i = 1; i <= need_map; ++i)//��һ���仺����
			*(finish_map_ptr + i) = data_alloc.allocate(buffersize);
		iterator new_finish = finish + n;//������ĩ�˵�����
		copy_backward(p, finish, new_finish);//����ƶ�Ԫ��
		fill_n(p, n, value);//��佫Ҫ����Ԫ��
		finish = new_finish;//����ĩ�˵�����
	}
	else
	{//������ǰ�ƶ�Ԫ�أ��������Գ�
		size_t front_remain_size = start.current - start.first;
		size_t need_map = (n - front_remain_size) / buffersize + 1;
		full_front(need_map);
		p = start + element_before;//�ؼ���ͬ��
		map_pointer start_map_ptr = start.ptr_map;
		for (size_t i = need_map; i >= 1; --i)
			*(start_map_ptr - i) = data_alloc.allocate(buffersize);
		iterator new_start = start - n;
		copy(start, p, new_start);
		fill_n(p - n, n, value);
		start = new_start;
	}
}

template <typename T, size_t buffersize> template <typename InputIterator>
void png_deque<T, buffersize>::insert(iterator p, InputIterator beg, InputIterator end)
{//������λ�ò���һ��Ԫ��[beg,end)��������������insert����ʵ���Ե�������insertʵ�֣�����Ч��̫��
 //ò��Ҳû�а취дһ�����ܺ��������ߵ��ã�һ���ǲ���һ��Ԫ�ص�n��������һ���ǲ���һ��Ԫ��
	size_t add_size = length(beg,end), deque_size = size(), element_before = p - start;
	if (element_before > deque_size / 2)
	{
		size_t back_remain_size = finish.last - finish.current;
		size_t need_map = back_remain_size > add_size ? 0: 
			((add_size - back_remain_size) / buffersize + 1);
		full_back(need_map);
		//�ǳ��ؼ���һ��������Ϊ��full_back����֮�󣬿����Ѿ����·������п�������ô��ζ��p�Ѿ�ʧЧ
		//�ϸ���˵��p�е�ptr_map����ָ���п�����ָ��ʧЧ���������Ҫ�����趨��
		p = start + element_before;
		map_pointer finish_map_ptr = finish.ptr_map;
		for (size_t i = 1; i <= need_map; ++i)
			*(finish_map_ptr + i) = data_alloc.allocate(buffersize);
		iterator new_finish = finish + add_size;
		copy_backward(p, finish, new_finish);
		copy(beg, end, p);
		finish = new_finish;
	}
	else
	{
		size_t front_remain_size = start.current - start.first;
		size_t need_map = front_remain_size > add_size ? 0 :
			((add_size - front_remain_size) / buffersize + 1);
		full_front(need_map);
		p = start + element_before;//�ؼ���ͬ��
		map_pointer start_map_ptr = start.ptr_map;
		for (size_t i = 1; i <= need_map; ++i)
			*(--start_map_ptr) = data_alloc.allocate(buffersize);
		iterator new_start = start - add_size;
		copy(start, p, new_start);
		copy_backward(beg, end, p);
		start = new_start;
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::resize(size_type n, const T &value = T())
{//����������С
	size_t deque_size = size();
	if (n < deque_size)//�´�С��Ŀǰ����sizeС
		erase(start + n, finish);//��ɾ��һ����Ԫ��
	else
	{//�������һ����Ԫ��
		for (size_t i = deque_size; i < n; ++i)
			push_back(value);
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::clear()
{//���������ע�������Ĭ����һ���ջ�����
	for (map_pointer curr = start.ptr_map + 1; curr < finish.ptr_map; ++curr)
	{//��λ��ͷβ�м�Ļ��������
		destroy(*curr, buffersize);
		data_alloc.deallocate(*curr, buffersize);
	}
	if (start.ptr_map != finish.ptr_map)
	{//���У�ʣ�£����������������������Ԫ�أ�������finish��ָ�������ռ�
		destroy(finish.first, finish.current - finish.first);
		destroy(start.current, start.last - start.current);
		data_alloc.deallocate(finish.first, buffersize);
	}
	else//��ֻ��һ��������Ԫ�ؼ���
		destroy(start.current, finish.current - start.current);
	finish = start;//���õ�����
}

template < typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::erase(iterator p)
{//ɾ��ָ��Ԫ��
	size_t element_before = p - start, deque_size = size();
	iterator next = p;
	++next;//��ɾԪ����һλ��
	if (element_before < deque_size / 2)
	{//��ǰ��Ԫ�ؽ��٣��������ƶ�
		copy_backward(start, p, next);
		pop_front();//ɾ������Ԫ��
	}
	else
	{//����
		copy(next, finish, p);
		pop_back();
	}
	return start + element_before;//����ɾ��Ԫ����һԪ�ص�����
}

template <typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::erase(iterator b, iterator e)
{//ɾ��һ��Ԫ��
	if (b == start && e == finish)
	{//��ɾ����������
		clear();
		return finish;
	}
	else
	{//����
		size_t element_before = b - start, length = e - b, deque_size = size();
		if (element_before < (deque_size - length) / 2)
		{//ǰ��Ԫ�ؽ��٣�������ƶ�
			copy_backward(start, b, e);
			iterator new_start = start + length;
			destroy(start.current, new_start.current - start.current);
			for (map_pointer curr = start.ptr_map; curr != new_start.ptr_map; ++curr)
				data_alloc.deallocate(*curr, buffersize);//��һ���ջ������ռ�
			start = new_start;//�������׶˵�����
		}
		else
		{//�����������Գ�
			copy(e, finish, b);
			iterator new_finish = finish - length;
			destroy(new_finish.current, finish.current - new_finish.current);
			for (map_pointer curr = new_finish.ptr_map + 1; curr <= finish.ptr_map; ++curr)
				data_alloc.deallocate(*curr, buffersize);
			finish = new_finish;
		}
		return start + element_before;//���ش˶�Ԫ����һλ�õ�����
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::pop_back()
{//�Ӷ�βɾ��Ԫ��
	if (finish.current != finish.first)
	{//���˻�������Ϊ��
		--finish;
		data_alloc.destroy(finish.current);
	}
	else
	{//����
		data_alloc.deallocate(finish.first, buffersize);//���մ˻�����
		finish.set_new_node(finish.ptr_map - 1);
		finish.current = finish.last - 1;
		data_alloc.destroy(finish.current);//������һ���������һ��Ԫ��
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::pop_front()
{//�Ӷ���ɾ��һ��Ԫ��

	if (start.current + 1 == start.last)
	{//���˻�������ʣһ��Ԫ��
		data_alloc.deallocate(start.first, buffersize);
		start.set_new_node(start.ptr_map + 1);
		start.current = start.first;
	}
	else
	{//����
		data_alloc.destroy(start.current);
		++start;
	}
}

template <typename T, size_t buffersize>
png_deque<T, buffersize>& png_deque<T, buffersize>::operator=(const png_deque &rhs)
{//���ظ��Ʋ�����
	clear();//�����
	insert(begin(),rhs.begin(), rhs.end());//�ٲ���
	return *this;
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::assign(size_type n, const T &value = T())
{//����
	clear();
	insert(begin(), n, value);
}

template <typename T, size_t buffersize> template <typename InputIterator>
void png_deque<T, buffersize>::assign(InputIterator b, InputIterator e)
{//���أ�����
	clear();
	insert(begin(), b, e);
}

#endif