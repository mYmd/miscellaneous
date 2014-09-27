//userliteral header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_USER_LITERAL_INCLUDED
#define MMYYMMDD_USER_LITERAL_INCLUDED

#include <ratio>

// create std::integral_constant and std::ratio from user defined literal
// ユーザー定義リテラルから std::integral_constant と std::ratio を出力する
namespace mymd	{

	namespace detail_user_literal	{

		constexpr std::size_t buncho(char x)
		{  return ('0' <= x && x <= '9')? x-'0': ('a' <= x && x <='f' )? x-'a'+10: ('A' <= x && x <='F' )? x-'A'+10: 0; }

		template <std::size_t, std::size_t, std::size_t, char...> struct digit;
    
		template <std::size_t L, std::size_t N, std::size_t D, char first, char... tail>
		struct digit<L, N, D, first, tail...>    {
			 static constexpr std::size_t num = digit<L, L * N + buncho(first), 10 * D, tail...>::num;
			 static constexpr std::size_t den = digit<L, L * N + buncho(first), 10 * D, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D, char... tail>
		struct digit<L, N, D, 'x', tail...>    {
			static constexpr std::size_t num = digit<16, 0, D, tail...>::num;
			static constexpr std::size_t den = digit<16, 0, D, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D, char... tail>
		struct digit<L, N, D, 'X', tail...>    {
			static constexpr std::size_t num = digit<16, 0, D, tail...>::num;
			static constexpr std::size_t den = digit<16, 0, D, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D, char... tail>
		struct digit<L, N, D, 'b', tail...>    {
			static constexpr std::size_t num = digit<2, 0, D, tail...>::num;
			static constexpr std::size_t den = digit<2, 0, D, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D, char... tail>
		struct digit<L, N, D, 'B', tail...>    {
			static constexpr std::size_t num = digit<2, 0, D, tail...>::num;
			static constexpr std::size_t den = digit<2, 0, D, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D, char... tail>
		struct digit<L, N, D, '.', tail...>    {
			static constexpr std::size_t num = digit<L, N, 1, tail...>::num;
			static constexpr std::size_t den = digit<L, N, 1, tail...>::den;
		};

		template <std::size_t L, std::size_t N, std::size_t D>
		struct digit<L, N, D>    {
			constexpr static std::size_t num = N;
			constexpr static std::size_t den = D;
		};

		template <char first, char... tail>
		constexpr int num()  { return digit<first=='0'? 8: 10, 0, 0, first, tail...>::num; }

		template <char first, char... tail>
		constexpr int den()
		{
			return digit<first=='0'? 8: 10, 0, 0, first, tail...>::den == 0? 1: digit<first=='0'? 8: 10, 0, 0, first, tail...>::den;
		}
	
	}	//namespace detail_user_literal

	namespace ul    {
		template <char... Char>
		constexpr auto operator "" _ic()
			->std::integral_constant<int, detail_user_literal::num<Char...>() / detail_user_literal::den<Char...>()>
		{
			return std::integral_constant<int, detail_user_literal::num<Char...>() / detail_user_literal::den<Char...>()>{};
		}

		template <char... Char>
		constexpr auto operator "" _rc()
			->std::ratio<detail_user_literal::num<Char...>(), detail_user_literal::den<Char...>()>
		{
			return std::ratio<detail_user_literal::num<Char...>(), detail_user_literal::den<Char...>()>{};
		}
    }	//	namespace ul

}	//	namespace mymd

#endif
