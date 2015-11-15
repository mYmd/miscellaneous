//sample of mbind header
//Copyright (c) 2014 mmYYmmdd

#include "mbind.hpp"
#include <iostream>
#include <string>
#include <tuple>

int main()
{
	using mymd::_x_;    // 型プレースホルダ
    // 完成型：5要素のtuple
    using tuple5 = std::tuple<char, std::string, int, char, int>;
    // 最初に型を3つだけセット
    using bind_3_5 = mymd::mbind<std::tuple, char, _x_, int, _x_, int>;
    // 残り２つの型を埋める
    {
        using bind_5_5 = bind_3_5::apply<std::string, char>;
        static_assert(std::is_same<bind_5_5, tuple5>::value, "!=");
    }
    // 残り２つの型をひとつずつ埋める
    {
        using mymd::_xrcv_;    // 型プレースホルダ（cv除去）
        using bind_4_5 = bind_3_5::apply<std::string, _xrcv_>;
        using bind_5_5 = bind_4_5::apply<const char>;
        static_assert(std::is_same<bind_5_5, tuple5>::value, "!=");
    }
    // operator ||, operator &&, operator !
    using mymd::_xrr_;
    using mymd::_xrcvr_;
    auto m1 = mymd::mbind<std::is_integral, _xrr_>{} || mymd::mbind<std::is_floating_point, _xrr_>{};
    auto m2 = !mymd::mbind<std::is_same, float, _xrr_>{};
    auto m3 = mymd::mbind<std::is_same, std::string, _xrcvr_>{};
    using b = decltype((m1 && m2) || m3);
    std::cout << b::apply<int>::value << std::endl;
    std::cout << b::apply<int*>::value << std::endl;
    std::cout << b::apply<short>::value << std::endl;
    std::cout << b::apply<unsigned char>::value << std::endl;
    std::cout << b::apply<double>::value << std::endl;
    std::cout << b::apply<float>::value << std::endl;
    std::cout << b::apply<long long&>::value << std::endl;
    std::cout << b::apply<const std::string&>::value << std::endl;
    return 0;
}
