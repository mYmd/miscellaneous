//sample of functor_overload header
//Copyright (c) 2014 mmYYmmdd

#include "functor_overload.hpp"
#include <iostream>

template <typename T>
    struct is_Numeric   {
        static const bool value = std::is_integral<typename std::remove_reference<T>::type>::value ||
                                std::is_floating_point<typename std::remove_reference<T>::type>::value;
};

template <typename T>
    struct is_Pointer   {
        static const bool value = std::is_pointer<typename std::remove_reference<T>::type>::value;
};

struct Plus {
    template <typename T>
        auto operator()(T a, T b) const ->decltype(a+b)
        { return a + b; }
    template <typename T>
        auto operator()(T* a, T* b) const ->decltype(*a+*b)
        { return *a + *b; }
};

struct Minus {
    template <typename T>
        auto operator()(T a, T b) const ->decltype(a-b)
        { return a - b; }
    template <typename T>
        auto operator()(T* a, T* b) const ->decltype(*a-*b)
        { return *a - *b; }
};

// オーバーロード関数を成長させる
template <typename... A, typename T, typename U, typename F>
constexpr auto add_overload(const mymd::overload_t<T, U>& x, F&& fun)
{
    return x + mymd::overload<A...>(std::forward<F>(fun));
}

void test_functor_overload()
{
    using namespace mymd;
    std::cout << "// sample for overload the calculations of numerics and of pointers" << std::endl;
    std::cout << "// sets of signatures of two functors are same,  " << std::endl;
    std::cout << "// and choice the members to use from them " << std::endl;
    std::cout << "//数値／ポインタに対する計算をオーバーロードする例" << std::endl;
    std::cout << "//ふたつのファンクタのシグネチャは同一。それを意識的に選択する。" << std::endl;
    auto m0 = overload<cond<is_Numeric>, cond<is_Numeric>>(Plus{} ) + 
              overload<cond<is_Pointer>, cond<is_Pointer>>(Minus{}) ;
    auto mm = add_overload<int, int*>(m0, [](auto i, auto* j){return i + *j;});
    int i = 4, j = 5;
    double a = 3.676, b = 8.3212;
    std::cout << mm(i, j) << std::endl;
    std::cout << mm(a, b+0.0) << std::endl;
    std::cout << mm(&i, &j) << std::endl;
    std::cout << mm(&a, &b) << std::endl;
    std::cout << mm(i, &j) << std::endl;
}
