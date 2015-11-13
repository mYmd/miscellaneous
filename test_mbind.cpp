//sample of mbind header
//Copyright (c) 2014 mmYYmmdd

#include "mbind.hpp"
#include <iostream>
#include <string>
#include <tuple>

void test_mbind()
{
	using mymd::_x_;	using mymd::_xrr_;
    {
        using mymd::_xrcv_;
        using tpl = mymd::mbind<std::tuple, char, _x_, int, _xrcv_, int>;
        auto m = tpl::apply<std::string, const char>{};
        std::get<0>(m) = 'k';
        std::get<1>(m) = "std::get<1>(m)";
        std::get<2>(m) = 3432;
        std::get<3>(m) = 'Z';
        std::get<4>(m) = 48;
        std::cout << std::get<0>(m) << std::endl;
        std::cout << std::get<1>(m) << std::endl;
        std::cout << std::get<2>(m) << std::endl;
        std::cout << std::get<3>(m) << std::endl;
        std::cout << std::get<4>(m) << std::endl;
    }
    {
        using mymd::_xrcvr_;
		auto m1 = mymd::mbind<std::is_integral, _xrr_>{} || mymd::mbind<std::is_floating_point, _xrr_>{};
		auto m2 = !mymd::mbind<std::is_same, float, _xrr_>{};
		auto m3 = mymd::mbind<std::is_same, std::string, _xrcvr_>{};
		using b = decltype(m1 && m2 || m3);
		std::cout << b::apply<int>::value << std::endl;
		std::cout << b::apply<int*>::value << std::endl;
		std::cout << b::apply<short>::value << std::endl;
		std::cout << b::apply<unsigned char>::value << std::endl;
		std::cout << b::apply<double>::value << std::endl;
		std::cout << b::apply<float>::value << std::endl;
		std::cout << b::apply<long long&>::value << std::endl;
		std::cout << b::apply<const std::string&>::value << std::endl;
	}
}
