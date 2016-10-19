###std::numeric_limits<unsigned int>::max()
- 返回编译器允许的unsigned int型数 最大值
- 需包含头文件 #include <limits>

###boost::dynamic_bitset
- bitset与vector<bool\>类似，同样存储二进制位，但它的大小固定，而且比vector<bool\>支持更多的位运算。
- vector<bool\>和bitset各有优缺点：vector<bool\>可以动态增长，但不能方便地进行位运算；bitset则正好相反，可以方便地对容纳的二进制位做位运算，但不能动态增长。
- boost::dynamic_bitset的出现恰好填补了这两者之间的空白，它std::bitset，提供丰富的位运算，同时长度又是动态可变的。
- 头文件 #include <boost/dynamic_bitset.hpp>

##
    //1.构造
    dynamic_bitset<> db1;
    dynamic_bitset<> db2(10);
    dynamic_bitset<> db3('0x16, BOOST_BINARY(10101)');
    dynamic_bitset<> db4(std::string("0101"));

    //2.resize
    db1.resize(8, true);
    assert(db1.to_ulong() == BOOST_BINARY(11111111));
    db1.resize(5);
    assert(db1.to_ulong() == BOOST_BINARY(11111));
    db1.clear();
    assert(db1.empty() && db1.size() == 0);

    //3.push_back
    //dynamic_bitset可以像vector那样使用push_back()向容器末尾（二进制头部）追加一个值
    dynamic_bitset<> db5(5, BOOST_BINARY(01010));
    assert(db5.to_ulong() == BOOST_BINARY(01010));
    db5.push_back(true);
    assert(db5.to_ulong() == BOOST_BINARY(101010));
    db5.push_back(false);
    assert()db5.to_ulong() == BOOST_BINARY(0101010));

    //4.block
    //dynamic_bitset使用block来存储二进制位，一个block就可以存储32个二进制位
    assert(dynamic_bitset<>(32).num_blocks() == 1);
    assert(dynamic_bitset<>(32).num_blocks() == 2);

    //5.位运算
    dynamic_bitset<> db6(4, BOOST_BINARY(1010));
    db6[0] &= 1;
    db6[1] ^= 1;
    db6[2] |= 1;
    assert(db6.to_ulong() == BOOST_BINARY_UL(1100));

    //6.访问元素
    dynamic_bitset<> db7(4, BOOST_BINARY(1100));
    assert(db7.test(0) && ！db7.test(1));
    assert(db7.any() && !db7.none())
    assert(db.count() == 2);  


-   dynamic_bitset中四个测试二进制位的函数
> test()函数检验第n位是否为1；
> 如果容器中存在二进制为1，那么any()返回true；
> 如果容器中不存在二进制位1，那么none()返回true；
> count()函数统计容器中所有值为1的元素的数量

- 三个翻转二进制位的函数
> set()函数可以置全部或者特定位置为1或0；
> reset*()可以置全部或者特定位置为0；
> flip()可以反转全部或者特定位置的值；

###万能转换器boost::lexical_cast
boost::lexical_cast为数值之间的转换提供了一揽子方案，比如：将一个字符串“123”转换为整数123
#
    std::string s = "123";
    int a = lexical_cast<int>(s);
这种方法非常简单，强烈建议使用！如果转换发生了意外，lexical_cast会抛出一个bad_lexical_cast异常，因此程序中需要对其进行捕捉。
#
    #include "stdafx.h"
    #include <iostream>
    #include <boost/lexical_cast.hpp>
    
    int main()
    {
        std::string s = "123";
        int a = boost::lexical_cast<int>(s);
        double b = boost::lexical_cast<double>(s);
 
        printf("%d/r/n", a+1);
        printf("%lf/r/n", b+1);

        try
        {
            int c = boost::lexical_cast<int>("wrong number");
        }
        catch(boost::bad_lexical_cast & e)
        {
            printf("%s/r/n", e.what());
        }
        return 0;
    }
  
> 可类比于stl中的stringstream