
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
	//诸类型别名
	typedef random_access_iterator_tag							iterator_category;//迭代器类型
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
	//deque的迭代器封装有四个指针
	friend class png_deque < T, buffersize>;
	T *first;
	T *last;
	T *current;
	map_pointer ptr_map;
public:
	deque_iterator() :first(NULL), last(NULL), current(NULL), ptr_map(NULL){}
	deque_iterator(pointer ptr) :current(ptr), first(NULL), last(NULL), ptr_map(NULL){}//没多大用
	void set_new_node(map_pointer new_node)
	{//设置迭代器所指的缓冲区
		ptr_map = new_node;
		first = *ptr_map;
		last = first + static_cast<difference_type>(buffersize);
	}
	//各种重载运算符
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
	{//只重载了小于、小于或等于
		return (ptr_map == rhs.ptr_map) ? (current < rhs.current) : (ptr_map < rhs.ptr_map);
	}
	bool operator<=(const iterator &rhs)const
	{
		return (*this < rhs) || (*this == rhs);
	}
	iterator& operator+=(difference_type n)
	{//这个重载是下述所有加或减的基础，注意n可正可负
		difference_type offset = (current - first) + n;
		difference_type map_offset = offset >= 0 ? (offset / buffersize) :
			(-static_cast<difference_type>((-offset - 1) / buffersize) - 1);//offset为负时，map偏移量的计算
		if (map_offset != 0)//不为0时说明所指缓冲区发生改变，需要调整
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

template <typename T, size_t buffersize = 10>//默认缓冲区大小为10
class png_deque
{
public:
	//各类型别名
	typedef T													          value_type;
	typedef size_t												          size_type;
	typedef ptrdiff_t											          difference_type;
	typedef typename deque_iterator<T, buffersize>::iterator			  iterator;
	typedef T&													          reference;
	typedef const T&											          const_reference;
private:
	typedef T**													map_pointer;
	typedef value_type*											pointer;
	iterator start;//指向第一个缓冲区，其中封装的current指向第一个元素
	iterator finish;//指向最后一个缓冲区，其中封装的current指向最末元素的下一位置
	map_pointer map;//指向所谓中控器，存储有各缓冲区地址
	size_type mapsize;//中控器大小
	static allocator<T> data_alloc;//元素内存分配器
	static allocator<pointer> map_alloc;//中控器内存分配器
	size_type get_mapsize()
	{//获得map大小，最初map大小为8.
		if (mapsize <= 0)
			mapsize = 8;
		return mapsize;
	}
	//以下三个函数很重要，负责最初构造容器对象的分配和初始化
	void fill_initialized(size_type, const T&);
	template <typename InputIterator>
	void fill_initialized(InputIterator, InputIterator);
	void alloc_map_and_buff(size_type);
	//以下函数负责缓冲区满时，具体的插入工作
	void push_back_aux(const T &);
	void full_back(size_type need_add_map = 1)
	{//判断中控器尾端是否还有足够剩余空间
		if (finish.ptr_map - map + need_add_map >= mapsize)
			reallocate_map(need_add_map, true);//不够则分配
	}
	void push_front_aux(const T&);
	void full_front(size_type need_add_map = 1)
	{//判断中控器前端是否还有剩余空间
		if (start.ptr_map - need_add_map < map)
			reallocate_map(need_add_map, false);
	}
	void reallocate_map(size_type need_add_map, bool full_at_back);
	//销毁p所指的n个对象
	void destroy(iterator p, size_type n)
	{
		for (size_t i = 0; i != n; ++i)
			data_alloc.destroy(p.current + i);
	}
public:
	//各种构造函数，最初的空容器默认有一个缓冲区
	png_deque() :start(), finish(), map(NULL), mapsize(0){ alloc_map_and_buff(0); }
	png_deque(const png_deque &rhs) { fill_initialized(rhs.begin(), rhs.end()); }
	template <typename InputIterator>
	png_deque(InputIterator b, InputIterator e) { fill_initialized(b, e); }
	png_deque(size_type n, const T &value = T()) { fill_initialized(n, value); }
	png_deque(int n, const T &value = T()) { fill_initialized(n, value); }
	png_deque(long n, const T &value = T()) { fill_initialized(n, value); }
	//begin和end操作
	iterator begin()const { return start; }
	iterator end()const { return finish; }
	//添加元素
	void push_back(const T&);
	void push_front(const T&);
	iterator insert(iterator, const T&);
	void insert(iterator, size_type, const T&);
	template <typename InputIterator>
	void insert(iterator, InputIterator, InputIterator);
	//容器大小操作
	size_type size()const { return finish - start; }
	bool empty()const { return start == finish; }
	void resize(size_type, const T & = T());
	//访问容器元素操作
	reference back()const { return *(--end()); }
	reference front()const { return *(begin()); }
	reference operator[](size_type n) { return *(start + n); }
	reference at(size_type n)/*const*/
	{
		if (finish <= (start + n))
			cout << endl << "out of range of deque!" << endl;
		return (*this)[n];
	}
	//删除元素
	iterator erase(iterator);
	iterator erase(iterator, iterator);
	void clear();
	void pop_back();
	void pop_front();
	//赋值操作
	png_deque& operator=(const png_deque&);
	void assign(size_type, const T & = T());
	template <typename InputIterator>
	void assign(InputIterator, InputIterator);
	//析构函数
	~png_deque()
	{
		clear();
		data_alloc.deallocate(start.first, buffersize);
		map_alloc.deallocate(map, mapsize);
	}
};

//定义两个内存分配器
template <typename T, size_t buffersize> allocator<T> png_deque<T, buffersize>::data_alloc;
template <typename T, size_t buffersize> allocator<typename png_deque<T, buffersize>::pointer>
	png_deque<T, buffersize>::map_alloc;

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::alloc_map_and_buff(size_type element_size)
{//负责最初的中控器分配和缓冲区分配
	size_t need_map = element_size / buffersize + 1;//所需缓冲区个数
	mapsize = (get_mapsize() > (need_map + 2)) ? get_mapsize() : need_map + 2;
	map = map_alloc.allocate(mapsize);
	map_pointer map_start = map + (mapsize - need_map) / 2;//设置中控器起始使用位置
	map_pointer map_finish = map_start + need_map - 1;
	for (map_pointer curr = map_start; curr <= map_finish; ++curr)
		*curr = data_alloc.allocate(buffersize);//分配缓冲区,将地址存入map
	//设置两个迭代器
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
		uninitialized_fill(*curr, *curr + buffersize, value);//用value初始化缓冲区
	uninitialized_fill(finish.first, finish.current, value);//可能最后剩余的几个，不足以填充满一个完整的缓冲区
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
		{//用[beg,end)这段元素初始化缓冲区
			uninitialized_copy(beg__, end__, *curr);
			beg__ = end__;
			end__ += buffersize;
		}
	}
	uninitialized_copy(beg__, end, finish.first);//同理，注意这里是beg__，不是end__
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_back(const T &value)
{//向队列末端插入元素
	if ((finish.current + 1) != finish.last)
	{//如果缓冲区空间足够，直接插入
		data_alloc.construct(finish.current, value);
		++finish.current;
	}
	else//否则交给它
		push_back_aux(value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_back_aux(const T &value)
{
	full_back();//判断中控器是否还有空间，没有则分配
	data_alloc.construct(finish.current, value);
	//后端又增加了一个缓冲区，重新调整finish迭代器
	*(finish.ptr_map + 1) = data_alloc.allocate(buffersize);
	finish.set_new_node(finish.ptr_map + 1);
	finish.current = finish.first;
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_front(const T &value)
{
	if (start.current != start.first)
	{//空间足够，则直接插入
		--start.current;
		data_alloc.construct(start.current, value);
	}
	else//否则交给它
		push_front_aux(value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::push_front_aux(const T &value)
{//类似push_back_aux
	full_front();
	*(start.ptr_map - 1) = data_alloc.allocate(buffersize);
	start.set_new_node(start.ptr_map - 1);
	start.current = start.last - 1;
	data_alloc.construct(start.current, value);
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::reallocate_map(size_type need_add_map, bool full_at_back)
{//若中控器后端或者前端空间不够
	size_t used_map = finish.ptr_map - start.ptr_map + 1;//中控器已使用大小
	size_t new_used_map = used_map + need_add_map;//将会使用的总大小
	map_pointer new_start_map;
	if (mapsize > 2 * new_used_map)
	{//中控器现存空间还足够
		new_start_map = map + (mapsize - new_used_map) / 2
			+ (full_at_back ? 0 : need_add_map);//重新调整中控器使用起始端
		if (full_at_back)//若后端满
			copy(start.ptr_map, finish.ptr_map + 1, new_start_map);
		else//若前端满
			copy_backward(start.ptr_map, finish.ptr_map + 1, new_start_map + used_map);
	}
	else
	{//现存空间不够，则重新分配更大的中控器
		size_t new_mapsize = mapsize + (mapsize > need_add_map ? mapsize : need_add_map) + 2;//计算新大小
		map_pointer new_map = map_alloc.allocate(new_mapsize);
		new_start_map = new_map + (new_mapsize - new_used_map) / 2 + (full_at_back ? 0 : need_add_map);//设置使用起始端
		copy(start.ptr_map, finish.ptr_map + 1, new_start_map);
		map_alloc.deallocate(map, mapsize);//释放原来的中控器
		//调整参数
		map = new_map;
		mapsize = new_mapsize;
	}
	start.set_new_node(new_start_map);
	finish.set_new_node(start.ptr_map + used_map - 1);
}

template <typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::insert(iterator p, const T &value)
{//任意位置插入一个元素
	if (p == start)
	{//若是起始处
		push_front(value);
		return start;
	}
	else if (p == finish)
	{//若是最尾端
		push_back(value);
		return (finish - 1);
	}
	else
	{//否则
		size_t element_before = p - start;//计算插入点前元素个数
		if (element_before > size() / 2)
		{//若前端元素较多，则向后移动后端元素
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			copy_backward(p, back2, back1);
		}
		else
		{//否则向前移动前端元素
			push_front(front());
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			copy(front2, p, front1);
			--p;
		}
		*p = value;//插入元素
		return p;
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::insert(iterator p, size_type n, const T &value)
{//任意位置插入n个元素value
	size_t deque_size = size(), element_before = p - start;
	if (element_before > deque_size / 2)
	{//前端元素较多，则向后移动元素
		size_t back_remain_size = finish.last - finish.current;//finish所指缓冲区还剩空间个数
		size_t need_map = (n - back_remain_size) / buffersize + 1;//需要再分配的缓冲区个数
		full_back(need_map);//判断中控器后端空间是否足够
		p = start + element_before;//关键，同下
		map_pointer finish_map_ptr = finish.ptr_map;
		for (size_t i = 1; i <= need_map; ++i)//逐一分配缓冲区
			*(finish_map_ptr + i) = data_alloc.allocate(buffersize);
		iterator new_finish = finish + n;//记下新末端迭代器
		copy_backward(p, finish, new_finish);//向后移动元素
		fill_n(p, n, value);//填充将要插入元素
		finish = new_finish;//更新末端迭代器
	}
	else
	{//否则，向前移动元素，与上述对称
		size_t front_remain_size = start.current - start.first;
		size_t need_map = (n - front_remain_size) / buffersize + 1;
		full_front(need_map);
		p = start + element_before;//关键，同下
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
{//在任意位置插入一段元素[beg,end)，类似上述函数insert，其实可以调用上述insert实现，但是效率太低
 //貌似也没有办法写一个功能函数供两者调用，一个是插入一个元素的n个副本，一个是插入一段元素
	size_t add_size = length(beg,end), deque_size = size(), element_before = p - start;
	if (element_before > deque_size / 2)
	{
		size_t back_remain_size = finish.last - finish.current;
		size_t need_map = back_remain_size > add_size ? 0: 
			((add_size - back_remain_size) / buffersize + 1);
		full_back(need_map);
		//非常关键的一步啊，因为在full_back函数之后，可能已经重新分配了中控器，那么意味着p已经失效
		//严格来说是p中的ptr_map，即指向中控器的指针失效，因而必须要重新设定！
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
		p = start + element_before;//关键，同上
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
{//重置容器大小
	size_t deque_size = size();
	if (n < deque_size)//新大小比目前容器size小
		erase(start + n, finish);//则删除一部分元素
	else
	{//否则插入一部分元素
		for (size_t i = deque_size; i < n; ++i)
			push_back(value);
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::clear()
{//清空容器，注意空容器默认有一个空缓冲区
	for (map_pointer curr = start.ptr_map + 1; curr < finish.ptr_map; ++curr)
	{//将位于头尾中间的缓冲区清除
		destroy(*curr, buffersize);
		data_alloc.deallocate(*curr, buffersize);
	}
	if (start.ptr_map != finish.ptr_map)
	{//若有（剩下）两个缓冲区，则继续析构元素，并回收finish所指缓冲区空间
		destroy(finish.first, finish.current - finish.first);
		destroy(start.current, start.last - start.current);
		data_alloc.deallocate(finish.first, buffersize);
	}
	else//若只有一个，析构元素即可
		destroy(start.current, finish.current - start.current);
	finish = start;//设置迭代器
}

template < typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::erase(iterator p)
{//删除指定元素
	size_t element_before = p - start, deque_size = size();
	iterator next = p;
	++next;//被删元素下一位置
	if (element_before < deque_size / 2)
	{//若前端元素较少，则往后移动
		copy_backward(start, p, next);
		pop_front();//删除队首元素
	}
	else
	{//否则
		copy(next, finish, p);
		pop_back();
	}
	return start + element_before;//返回删除元素下一元素迭代器
}

template <typename T, size_t buffersize>
typename png_deque<T, buffersize>::iterator png_deque<T, buffersize>::erase(iterator b, iterator e)
{//删除一段元素
	if (b == start && e == finish)
	{//若删除整个容器
		clear();
		return finish;
	}
	else
	{//否则
		size_t element_before = b - start, length = e - b, deque_size = size();
		if (element_before < (deque_size - length) / 2)
		{//前端元素较少，则向后移动
			copy_backward(start, b, e);
			iterator new_start = start + length;
			destroy(start.current, new_start.current - start.current);
			for (map_pointer curr = start.ptr_map; curr != new_start.ptr_map; ++curr)
				data_alloc.deallocate(*curr, buffersize);//逐一回收缓冲区空间
			start = new_start;//设置新首端迭代器
		}
		else
		{//否则，与上述对称
			copy(e, finish, b);
			iterator new_finish = finish - length;
			destroy(new_finish.current, finish.current - new_finish.current);
			for (map_pointer curr = new_finish.ptr_map + 1; curr <= finish.ptr_map; ++curr)
				data_alloc.deallocate(*curr, buffersize);
			finish = new_finish;
		}
		return start + element_before;//返回此段元素下一位置迭代器
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::pop_back()
{//从队尾删除元素
	if (finish.current != finish.first)
	{//若此缓冲区不为空
		--finish;
		data_alloc.destroy(finish.current);
	}
	else
	{//否则
		data_alloc.deallocate(finish.first, buffersize);//回收此缓冲区
		finish.set_new_node(finish.ptr_map - 1);
		finish.current = finish.last - 1;
		data_alloc.destroy(finish.current);//析构上一缓冲区最后一个元素
	}
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::pop_front()
{//从队首删除一个元素

	if (start.current + 1 == start.last)
	{//若此缓冲区仅剩一个元素
		data_alloc.deallocate(start.first, buffersize);
		start.set_new_node(start.ptr_map + 1);
		start.current = start.first;
	}
	else
	{//否则
		data_alloc.destroy(start.current);
		++start;
	}
}

template <typename T, size_t buffersize>
png_deque<T, buffersize>& png_deque<T, buffersize>::operator=(const png_deque &rhs)
{//重载复制操作符
	clear();//先清空
	insert(begin(),rhs.begin(), rhs.end());//再插入
	return *this;
}

template <typename T, size_t buffersize>
void png_deque<T, buffersize>::assign(size_type n, const T &value = T())
{//重置
	clear();
	insert(begin(), n, value);
}

template <typename T, size_t buffersize> template <typename InputIterator>
void png_deque<T, buffersize>::assign(InputIterator b, InputIterator e)
{//重载，重置
	clear();
	insert(begin(), b, e);
}

#endif