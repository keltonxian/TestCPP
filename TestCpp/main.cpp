#include <iostream>
#include <vector>
#include <future>
#include <functional>
#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;

template<typename T, typename U>
auto Add(T t, U u) -> decltype(t + u)
{
    return t + u;
}

class TestA
{
public:
    TestA(int size) : size_(size)
    {
        data_ = new int[size];
    }
    TestA(){}
    ~TestA()
    {
       if (nullptr != data_)
       {
           delete[] data_;
       }
    }

    //copy
    TestA(const TestA& a)
    {
        size_ = a.size_;
        data_ = new int[size_];
        cout << "copy" << endl;
    }

    //move
    TestA(TestA&& a)
    {
        data_ = a.data_;
        a.data_ = nullptr;
        cout << "move" << endl;
    }

    int *data_;
    int size_;
};

void PrintV(int &t)
{
    cout << "lvalue" << endl;
}

void PrintV(int &&t)
{
    cout << "rvalue" << endl;
}

template<typename T>
void TestB(T &&t)
{
    PrintV(t);
    PrintV(std::forward<T>(t));
    PrintV(std::move(t));
}

struct StA
{
    int a;
    int b;
    int c;
    StA(int, int){}
};

struct StB
{
    StB(int){}
private:
    StB(const StB&){}
};

struct StC
{
    std::vector<int> data;
    StC(std::initializer_list<int> list)
    {
        for (auto iter = list.begin(); iter != list.end(); ++iter)
        {
            data.push_back(*iter);
        }
    }
};

std::function<void(int)> TestFunc;
struct Foo
{
    Foo() {}
    Foo(int num) : num_(num) {}
    void print_add(int i) const { cout << num_ + i << '\n'; }
    void print_sum(int n1, int n2) { cout << n1 + n2 << endl; }
    int num_;
    int data_ = 10;
};

void print_num(int i) { cout << i << '\n'; }

struct PrintNum
{
    void operator()(int i) const { cout << i << '\n'; }
};

void TestF2(int n1, int n2, int n3, const int& n4, int n5)
{
    cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << endl;
}

int TestG(int n1) { return n1; }

struct SimpleA
{
    int a;
    int b;
};

// template with default argument type
template <typename T, typename U=int>
class TempA
{
    T value;
};

// 类模板的默认模板参数必须从右往左定义，而函数模板则没有这个限制。
template <typename R, typename U=int>
R TempFunc1(U val) {
    return val;
}

template <typename R=int, typename U>
R TempFunc2(U val) {
    return val;
}

// 可变长参数模板
template <typename T>
void TempFunc3(const T& t){
    cout << t << '\n';
}

template <typename T, typename ... Args>
void TempFunc4(const T& t, Args ... args){
    cout << t << ',';
    func(args...);
}

int main() {
    cout << "Hello, World!" << endl;

    auto func = [&]
    {
        cout << "auto lambda";
    };
    //auto asyncFunc = std::async(std::launch::async, func);

    int a = 0, b = 0;
    decltype(a + b) c = 0;
    decltype(a += b) d = c;
    d = 20;
    cout << "C:" << c << endl;

    cout << "AddTest 1:" << Add(1, 3) << endl;
    cout << "AddTest 2:" << Add(3.3, 5.2) << endl;

    TestA a1(10);
    TestA b1 = a1;
    TestA c1 = std::move(a1);
    cout << "TestA a " << a1.data_ << endl;
    cout << "TestA b " << b1.data_ << endl;
    cout << "TestA c " << c1.data_ << endl;

    StB stB1{123};
    StB stB2 = {123};
    StA stA{1, 3};
    StC stC{1, 2, 3, 4, 5, 6};

    TestB(1);
    int a2 = 1;
    TestB(a2);
    TestB(std::forward<int>(a2));
    TestB(std::forward<int&>(a2));
    TestB(std::forward<int&&>(a2));

    // 储存自由函数
    std::function<void(int)> f_display = print_num;
    f_display(-9);

    // 储存 lambda
    std::function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();

    // 储存到 std::bind 调用的结果
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();

    // 储存到成员函数的调用
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);

    // 储存到数据成员访问器的调用
    std::function<int(Foo const&)> f_num = &Foo::num_;
    cout << "num_: " << f_num(foo) << '\n';

    // 储存到成员函数及对象的调用
    std::function<void(int)> f_add_display2 = std::bind(&Foo::print_add, &foo, placeholders::_1);
    f_add_display2(2);

    // 储存到成员函数和对象指针的调用
    std::function<void(int)> f_add_display3 = std::bind(&Foo::print_add, foo, placeholders::_1);
    f_add_display3(3);

    // 储存到函数对象的调用
    std::function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);

    // 演示参数重排序和按引用传递
    int n = 7;
    auto f1 = std::bind(TestF2, placeholders::_2, 42, placeholders::_1, std::cref(n), n);
    n = 10;
    // 1 为 _1 所绑定， 2 为 _2 所绑定，不使用 1001
    // 进行到 f(2, 42, 1, n, 7) 的调用
    f1(1, 2, 1001);

    // 嵌套 bind 子表达式共享占位符
    auto f2 = std::bind(TestF2, placeholders::_3, std::bind(TestG, placeholders::_3), placeholders::_3, 4, 5);
    // 进行到 f(12, g(12), 12, 4, 5); 的调用
    f2(10, 11, 12);

    // 绑定指向成员函数指针
    Foo foo2;
    auto f3 = std::bind(&Foo::print_sum, &foo2, 95, placeholders::_1);
    f3(5);

    // 绑定指向数据成员指针
    auto f4 = std::bind(&Foo::data_, placeholders::_1);
    cout << f4(foo2) << endl;

    // 智能指针亦能用于调用被引用对象的成员
    cout << f4(std::make_shared<Foo>(foo2)) << endl;

    /**
     * lambda
     * auto func = [capture] (params) opt -> ret { func_body; };
     * []不捕获任何变量
     * [&]引用捕获，捕获外部作用域所有变量，在函数体内当作引用使用
     * [=]值捕获，捕获外部作用域所有变量，在函数内内有个副本使用
     * [=, &a]值捕获外部作用域所有变量，按引用捕获a变量
     * [a]只值捕获a变量，不捕获其它变量
     * [this]捕获当前类中的this指针
     */

    auto func1 = [](int a) -> int { return a + 1; };
    auto func2 = [](int a) { return a + 2; };
    cout << func1(1) << ' ' << func2(2) << endl;

    int a11 = 0;
    auto f11 = [=]() { return a11; };
    cout << f11() << endl;

    auto f22 = [=]() mutable { return a11++; }; //修改值传递变量，用mutable
    cout << "f22:" << f22() << endl;
    cout << "a11:" << a11 << endl;

    vector<SimpleA> vecSimpleA;
    std::sort(vecSimpleA.begin(), vecSimpleA.end(), [](const SimpleA& left, const SimpleA& right)
    {
        return left.a < right.a;
    });

    vector<int> vecInt;
    for (int i = 0; i < 6; i++)
    {
        vecInt.push_back(i);
    }

    // template
    typedef void (*func31)(int, int); // before 11
    using func32 = void (*)(int, int); // after 11

    // before C++ 11
    cout << "vector loop before C++ 11" << endl;
    for (auto iter = vecInt.begin(); iter != vecInt.end(); iter++)
    {
        cout << *iter << endl;
    }

    // after C++ 11
    cout << "vector loop after C++ 11" << endl;
    for (int i : vecInt)
    {
        cout << i << endl;
    }
    return 0;
}
