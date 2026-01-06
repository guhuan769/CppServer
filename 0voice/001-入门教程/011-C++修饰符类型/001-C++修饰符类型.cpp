//C++ 允许在 char、int 和double 数据类型前放置修饰符。修饰符用于改变基本类型的含义，所以它更能满足各种情镜的需求。
// 下面列出了数据类型修饰符:
// * signed * unsigned * long * short
// 修饰符i signed、unsigned、long和short 可应用于整型，signed和unsigned可应用于字符型，long可应用于双精度型。
// 修饰符signed和unsigned也可以作为long  或 short 修饰符的前缀。列如：unsigned long int .
// C++ 允许使用速记符号来声明无符号短整数 或 无符号长整数。你可以不写 int,只写单词
   // unsigned、short 或 unsigned、 long 、int 是隐含的。列入，下面两个语句都声明了无符号整型变量。
#include<iostream>
using namespace std;

/*
* 这个程序演示了有符号整数和无符号整数之间的差别
*/

int main()
{
  short  int i; //有符号短整数
  short unsigned int j; //无符号短整数

  j = 50000;

  i = j;
  cout << i << " " << j;

  return 0;
}

