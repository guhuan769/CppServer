#include "lib.hpp"
#include <iostream>

using namespace std;

int Foo(int x,int y);//函数声明
inline int Foo(int x,int y) //函数定义
{
	return x + y;
}

// 定义处加inline 关键字，推荐这种写法！
inline void A::f1(int x){
	
}

int main()
{
	cout << Foo(1,2) << endl;
}
