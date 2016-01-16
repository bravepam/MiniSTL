
/*******************************************
 * Author: bravepam
 * E-mail:1372120340@qq.com
 * Blog:http://blog.csdn.net/zilingxiyue
 *******************************************
 */

#ifndef PNG_STACK_H
#define PNG_STACK_H

/*桟适配器，默认以png_vector(即双端队列)为底层容器，当然，
*也可以随便指定容器，不过这样的容器必须能够提供有助于实现桟接口的函数
*/

#include"png_vector.h"
#include<iostream>

using namespace std;

template <typename T,typename sequence = png_vector<T> >
class png_stack
{
public:
	typedef typename sequence::value_type			value_type;
	typedef typename sequence::size_type			size_type;
	typedef typename sequence::reference			reference;
	typedef typename sequence::const_reference		const_reference;
	typedef sequence								container_type;
private:
	friend bool operator< <T,sequence>(const png_stack&, const png_stack&);
	friend bool operator<= <T,sequence>(const png_stack&, const png_stack&);
	friend bool operator> <T,sequence>(const png_stack&, const png_stack&);
	friend bool operator>= <T,sequence>(const png_stack&, const png_stack&);
	friend bool operator== <T,sequence>(const png_stack&, const png_stack&);
	friend bool operator!= <T,sequence>(const png_stack&, const png_stack&);
private:
	sequence stk;
public:
	png_stack() :stk(){}
	png_stack(const png_stack &s) :stk(s.stk){}
	template <typename InputIterator>
	png_stack(InputIterator beg, InputIterator end) : stk(beg, end){}
	bool empty()const { return stk.empty(); }
	size_type size()const { return stk.size(); }
	void pop() { stk.pop_back(); }
	reference top() { return stk.back(); }
	const_reference top() const { return stk.back(); }
	void push(const T &value) { stk.push_back(value); }
};

template <typename T,typename sequence> 
bool operator<(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return lhs.stk < rhs.stk;
}

template <typename T,typename sequence> 
bool operator==(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return lhs.stk == rhs.stk;
}

template <typename T,typename sequence> 
bool operator<=(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return (lhs < rhs) || (lhs == rhs);
}

template <typename T,typename sequence> 
bool operator>(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return !(lhs <= rhs);
}

template <typename T,typename sequence> 
bool operator>=(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return !(lhs < rhs);
}

template <typename T,typename sequence> 
bool operator!=(const png_stack<T,sequence> &lhs, const png_stack<T,sequence> &rhs)
{
	return !(lhs == rhs);
}

#endif