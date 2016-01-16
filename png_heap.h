
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_MAX_HEAP_H
#define PNG_MAX_HEAP_H

/*以vector为底层默认容器的堆，默认最大堆
 */

#include<vector>
#include<iostream>

using namespace std;

template <typename T, typename CompareKey = less<T>, class GetKey = identity<T>,
	typename base_container = vector<T> >
class png_heap
{
public:
	typedef CompareKey											key_equal;
	typedef GetKey												get_key;
	typedef typename base_container::value_type					value_type;
	typedef typename base_container::size_type					size_type;
	typedef typename base_container::reference					reference;
	typedef typename base_container::const_reference			const_reference;
	typedef base_container										container_type;
private:
	base_container vec;
	key_equal compare;
	get_key getKey;
	void push_heapify(size_type);//插入调整
	void pop_heapify(size_type, size_type);//删除调整
	void make_heap();//建堆
	void swap(T &lhs, T &rhs)
	{
		T tmp = lhs;
		lhs = rhs;
		rhs = tmp;
	}
public:
	png_heap() :vec(), compare(key_equal()), getKey(get_key()){}
	png_heap(size_t n) :vec(n), compare(key_equal()), getKey(get_key()){}
	png_heap(size_t n, const key_equal &ke) :vec(n), compare(ke), getKey(get_key()){}
	png_heap(size_t n, const key_equal &ke, const get_key &gt) :vec(n), compare(ke), getKey(gt){}
	png_heap(const key_equal &ke, const get_key &gt = get_key()) :vec(), compare(ke), getKey(gt){}
	png_heap(const png_heap &heap) :vec(heap.vec), compare(heap.compare), getKey(heap.getKey){}
	png_heap& operator=(const png_heap &rhs)
	{
		if (&rhs != this)
		{
			vec = rhs.vec;
			compare = rhs.compare;
			getKey = rhs.getKey;
		}
		return *this;
	}
	png_heap(const base_container &con)
		:vec(con.begin(), con.end()), compare(key_equal()), getKey(get_key()){
		make_heap();
	}
	png_heap(const base_container &con,const key_equal &ke,const get_key &gt)
		:vec(con.begin(), con.end()), compare(ke),getKey(gt){
		make_heap();
	}
	template <typename InputIterator>
	png_heap(InputIterator beg, InputIterator end)
		: vec(beg, end){
		make_heap();
	}
	template <typename InputIterator>
	png_heap(InputIterator beg, InputIterator end, const key_equal &ke, const get_key & gt)
		: vec(beg, end), compare(ke), getKey(gt){
		make_heap();
	}
	bool empty() const { return vec.empty(); }
	size_type size()const { return vec.size(); }
	void push(const T &value)
	{//插入数据
		vec.push_back(value);
		push_heapify(size() - 1);
	}
	void pop()
	{//删除数据
		vec[0] = vec.back();
		vec.pop_back();
		if (size() != 0)
			pop_heapify(0, size() - 1);
	}
	reference top() { return vec[0]; }//返回最值
	void heap_sort();//堆排序，调用该函数后，堆结构将被破坏
	inline void print(ostream & = cout);//输出数据
};

template <typename T, typename CompareKey, class GetKey,typename base_container>
void png_heap<T, CompareKey, GetKey, base_container>::pop_heapify(size_type index, size_type heap_max_index)
{
	size_t left = 2 * index + 1, right = 2 * index + 2;
	while (left <= heap_max_index)
	{
		size_t max_index = index;
		if (compare(getKey(vec[index]), getKey(vec[left])))//在当前元素和左孩子中判断最优值
			max_index = left;//若左孩子最优
		if (right <= heap_max_index && 
			compare(getKey(vec[max_index]), getKey(vec[right])))//在当前元素和右孩子中判断最优值
			max_index = right;
		if (max_index != index)
		{//若最优元素有所改变，则交换，继续调整
			swap(vec[index], vec[max_index]);
			index = max_index;
			left = 2 * index + 1, right = 2 * index + 2;
		}
		else break;//否则退出
	}
}

template <typename T, typename CompareKey, class GetKey, typename base_container>
void png_heap<T, CompareKey, GetKey, base_container>::push_heapify(size_type index)
{//插入元素后调整
	size_t parent = (index - 1) >> 1;//该元素父亲
	T tmp = vec[index];
	while (index > 0 && compare(getKey(vec[parent]), getKey(tmp)))
	{//如果当前不是根部，且该节点比父节点更优
		vec[index] = vec[parent];
		index = parent;
		parent = (index - 1) >> 1;
	}
	vec[index] = tmp;
}

template <typename T, typename CompareKey, class GetKey, typename base_container>
void png_heap<T, CompareKey, GetKey, base_container>::make_heap()
{
	if (size() == 1) return;
	size_t heap_max_index = size() - 1;
	for (int i = (heap_max_index - 1) / 2; i >= 0; --i)
		pop_heapify(i, heap_max_index);
}

template <typename T, typename CompareKey, class GetKey, typename base_container>
void png_heap<T, CompareKey, GetKey, base_container>::heap_sort()
{
	size_t heap_max_index = size() - 1;
	for (size_t i = size() - 1; i != 0; --i)
	{
		swap(vec[0], vec[i]);//抽取最优元素
		pop_heapify(0, --heap_max_index);//调整堆
	}
}

template <typename T, typename CompareKey, class GetKey, typename base_container>
void png_heap<T, CompareKey, GetKey, base_container>::print(ostream &out = cout)
{
	for (size_type i = 0; i != size(); ++i)
	{
		out << vec[i];
		if (i != size() - 1) out << ' ';
		else
			out << endl;
	}
}

#endif
