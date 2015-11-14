//sample of mbind header
//Copyright (c) 2014 mmYYmmdd

#include "mbind.hpp"
#include <iostream>
#include <string>
#include <tuple>

int main()
{
    using mymd::_x_;	using mymd::_xrr_;
    using mymd::_xrcv_;
    using type0 = mymd::mbind<std::tuple, char, _x_, int, _xrcv_, int>;
    using type1 = type0::apply<std::string, const char>;
    static_assert(std::is_same<type1, std::tuple<char, std::string, int, char, int>>::value, "!=");
    //
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
