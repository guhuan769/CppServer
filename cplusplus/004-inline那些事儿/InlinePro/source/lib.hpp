#pragma once

#include <string>

/**
 * @brief The core implementation of the executable
 *
 * This class makes up the library part of the executable, which means that the
 * main logic is implemented here. This kind of separation makes it ecause the logic is nicely
 * separated from the comman  */

class A {
public:
	void f1(int x);
	void Foo(int x , int y ) // 定义即隐式内联函数！
       {

	};
	void f2(int x);//声明后，要想成为内联函数，必须定义处加inline关键字。
}
