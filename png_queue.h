
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_QUEUE_H
#define PNG_QUEUE_H

/*������������Ĭ����png_deque(��˫�˶���)Ϊ�ײ���������Ȼ��
 *Ҳ�������ָ���������������������� �����ܹ��ṩ������ʵ�ֶ��нӿڵĺ���
 */

#include"png_deque.h"

using namespace std;

template <typename T,typename sequence = png_deque<T> >
class png_queue
{
public:
	typedef typename sequence::size_type							size_type;
	typedef typename sequence::value_type							value_type;
	typedef typename sequence::reference							reference;
	typedef typename sequence::const_reference						const_reference;
	typedef sequence												container_type;
private:
	sequence que;
public:
	png_queue() :que(){}
	png_queue(const png_queue &q) :que(q.que){}
	template <typename InputIterator>
	png_queue(InputIterator beg, InputIterator end) : que(beg, end){}
	bool empty() const { return que.empty(); }
	size_type size() const { return que.size(); }
	void pop() { que.pop_front(); }
	reference front() { return que.front(); }
	const_reference front() const { return que.front(); }
	reference back() { return que.back(); }
	const_reference back()const { return que.back(); }
	void push(const T &value) { que.push_back(value); }
};

#endif