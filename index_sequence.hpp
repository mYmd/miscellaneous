//index_sequence header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_INDEX_SEQUENCE_INCLUDED
#define MMYYMMDD_INDEX_SEQUENCE_INCLUDED

#include <type_traits>

namespace mymd	{
	template <std::size_t... indices>
	struct index_sequence	{
		static constexpr std::size_t size()	{	return sizeof...(indices);	}
	};
	//------------------------------------------------
	template <typename T1, typename T2>	struct index_cat;

	template <std::size_t... indices1, std::size_t... indices2>
	struct index_cat<index_sequence<indices1...>, index_sequence<indices2...>>	{
		using type = index_sequence<indices1..., indices2...>;
	};

	template <std::size_t N, typename T>	struct index_shift;

	template <std::size_t N, std::size_t... indices>
	struct index_shift<N, index_sequence<indices...>>	{
		using type = index_sequence<N + indices...>;
	};
	//------------------------------------------------------------------------
	namespace detail_index_range_i	{
		template <std::size_t first, std::size_t len>
		struct index_range_i		{
			using type = typename index_shift<first, typename index_range_i<0, len>::type>::type;
		};

		template <std::size_t len>
		struct index_range_i<0, len>		{
			static constexpr std::size_t h = len/2;
			using type = typename index_cat<typename index_range_i<0, h>::type	,
									typename index_range_i<h, len-h>::type
							>::type;
		};

		template <> struct index_range_i<0, 0>	{ using type = index_sequence<>; };
		template <> struct index_range_i<0, 1>	{ using type = index_sequence<0>; };
		template <> struct index_range_i<0, 2>	{ using type = index_sequence<0, 1>; };
	}
	//+**************************************************************
	template <std::size_t first, std::size_t last>
	using index_range = typename detail_index_range_i::index_range_i<first, (first<last)? last - first: 0>::type;

	namespace detail_index_at	{
		//  get N_th index of a sequence of indexs
		template <std::size_t , std::size_t >		struct index_at	{	};

		template <typename , std::size_t...>		struct apart_i;

		template <std::size_t... num, std::size_t... indices>
		struct apart_i<index_sequence<num...>, indices...> : index_at<num, indices>...	{	};

		//  get N_th type of a sequence of types
		template <std::size_t , typename >	struct type_at	{	};

		template <typename , typename...>	struct apart_t;

		template <std::size_t... num, typename... types>
		struct apart_t<index_sequence<num...>, types...> : type_at<num, types>...		{	};

		template <std::size_t N>
		struct acceptor	{
			template <std::size_t i>
			static constexpr std::size_t upcast(const index_at<N, i>&)	{	return i;	}
			template <typename T>
			static T upcast(const type_at<N, T>&);
		};

		//  get N_th type of a sequence of types			att<N>(tuple);
		template <std::size_t , typename>	struct att_imple;

		template <std::size_t N, template <typename...> class tuple_t, typename... T>
		struct att_imple<N, tuple_t<T...>>    {
			using type_imple = apart_t<index_range<0, sizeof...(T)>, T...>;
			typedef decltype(acceptor<N>::upcast(type_imple{}))			type;
		};
	}	//namespace detail_index_at	

	//+**************************************************************
	//  get to N_th index of a sequence of indexs		at<N>(index_sequence);
	template <std::size_t N, template <std::size_t...> class index_tuple_t, std::size_t... indices>
	constexpr std::size_t ati(const index_tuple_t<indices...>& )
	{
		using type_imple = detail_index_at::apart_i<index_range<0, sizeof...(indices)>, indices...>;
		return detail_index_at::acceptor<N>::upcast(type_imple{});
	}

	//template <std::size_t N, template <std::size_t...> class index_tuple_t, std::size_t... indices>
	//constexpr std::size_t at = ati<N>(index_tuple_t<indices...>{});

	template <std::size_t N, typename T>
	using att = typename detail_index_at::att_imple<N, T>::type;
}

namespace mymd  {
	namespace detail_reverse	{
		template <typename...>	struct reverse_make;
		
		template <template <typename...> class types, typename...result, typename first, typename... tail>
		struct reverse_make<types<result...>, first, tail...> : reverse_make<types<first, result...>, tail...> { };

		template <template <typename...> class types, typename...result>
		struct reverse_make<types<result...>>		{
			using type = types<result...>;
		};

		template <typename T>	struct reverse_imple;

		template <template <typename...> class types, typename...E>
		struct reverse_imple<types<E...>>	{
			using type = typename reverse_make<types<>, E...>::type;
		};
	}
	// reverse the order of some tuple type  --  tupleなどの型の集合を逆順にする
	template <typename T>
	using reverse_t = typename detail_reverse::reverse_imple<
								typename std::remove_cv<typename std::remove_reference<T>::type>::type>::type;
}

namespace mymd  {
    namespace detail_count_template_parameters    {
        //　取りうるテンプレートパラメータの数を取得する関数 count<T> を実装したい
        //  例） count<std::is_integral> => 1  ,  count<std::pair> => 2  ,  count<std::tuple> => (size_t)-1(とりあえず...)
        //       count<quote<std::is_base_of>> => 2

        //template <template <typename...F> class>                      エラーになる
          //  constexpr std::size_t count1() { return sizeof...(F); }    NG! こうは書けない。Fを与えてないからしょうがない。
        //手で展開するしかないのか・・・
        template <template <typename> class>
            constexpr std::size_t count1() { return 1; }
        template <template <typename, typename> class>
            constexpr std::size_t count1() { return 2; }
        template <template <typename, typename, typename> class>
            constexpr std::size_t count1() { return 3; }
        template <template <typename, typename, typename, typename> class>
            constexpr std::size_t count1() { return 4; }
        template <template <typename, typename, typename, typename, typename> class>
            constexpr std::size_t count1() { return 5; }
        template <template <typename, typename, typename, typename, typename, typename> class>
            constexpr std::size_t count1() { return 6; }
        template <template <typename, typename, typename, typename, typename, typename, typename> class>
            constexpr std::size_t count1() { return 7; }
        // これ以上のパターンもひたすら定義する（しかし実用的にはここまでで十分のはず・・・？）
        //----------------------------------------------------------------------------
          //SFINAE    count2
        template <template <typename...> class T>
            constexpr std::size_t count2(decltype(count1<T>()))
			{ return count1<T>(); }
        template <template <typename...> class T>
            constexpr std::size_t count2(...)
			{ return (std::size_t)-1; }
        //---------------------
        template <typename T>
            constexpr std::size_t count2(decltype(count1<T::template apply>()))
			{ return count1<T::template apply>(); }
        template <typename T>
            constexpr std::size_t count2(...)
			{ return (std::size_t)-1; }
        //-------------------------------------------------------------------------------------
    }	//detail_count_template_parameters

	//関数 count
    template <template <typename...> class T>
	constexpr std::size_t	count_template_parameters()
	{	return detail_count_template_parameters::count2<T>(0);	}

	template <typename T>
	constexpr std::size_t  count_template_parameters()
	{	return detail_count_template_parameters::count2<T>(0);	}

	//variable templateに対してはどう書けばいいの？
    //template <??????????>
    //constexpr std::size_t    count();
}

//  Metaなbind    mbind
namespace mymd  {
	namespace detail_meta_bind  {
        //-----------------------------------------------
		//  default converter
        template <typename T>
			struct no_cnv  { using type = T; };
        //-----------------------------------------------
		//  placeholder
		template <template <typename> class>
			struct _X_ { };
		//-----------------------------------------------
		template <typename>
		struct voiD_t  { using type = void; };
		//-----------------------------------------------
		template <typename T, typename = void>
		struct has_type	{ using type = T; };

		template <typename T>
		struct has_type<T, typename voiD_t<typename T::type>::type>
		{ using type = typename T::type; };
		//-----------------------------------------------
		template <typename T>
		struct trap_space {
			static const bool value = false;
			template <typename>
				using conv = T;	//not convert
		};

		template <template <typename> class C>
		struct trap_space<_X_<C>> {
			static const bool value = true;
			template <typename V>
				using conv = typename has_type<C<V>>::type;	//convert
		};
		//-----------------------------------------------
		template <typename...>	struct types  { };
		//-----------------------------------------------
		template <typename, typename, typename = types<>> struct replace_space;
		template <typename T1, typename...T2, typename V1, typename... V2, typename...R>
		struct replace_space<types<T1, T2...>, types<V1, V2...>, types<R...>>	{
			using result = types<R...>;
			using alt_t  = typename trap_space<T1>::template conv<V1>;
			using base_v = typename std::conditional<trap_space<T1>::value, types<V2...>, types<V1, V2...>>::type;
			using base_t = replace_space<types<T2...>, base_v, types<R..., alt_t>>;
			using type = typename base_t::type;
		};
		template <typename...T, typename...R>
		struct replace_space<types<T...>, types<>, types<R...>>
		{ using type = types<R...>; };
		//-----------------------------------------------
		template <template <typename...> class, typename>	struct m_pack;
		template <template <typename...> class mata, typename...V>
		struct m_pack<mata, types<V...>>
		{ using type = mata<V...>; };

	}

	using detail_meta_bind::_X_;
	using _x_ = detail_meta_bind::_X_<detail_meta_bind::no_cnv>;

	template <template <typename...> class mata, typename... binder>
	class mbind	{
		template <typename... V>
		struct apply_imple	{
			using S = typename detail_meta_bind::replace_space<detail_meta_bind::types<binder...>, detail_meta_bind::types<V...>>::type;
			using type = typename detail_meta_bind::m_pack<mata, S>::type;
		};
	public:
		template <typename... V>
		using apply = typename apply_imple<V...>::type;
	};
}

#endif
