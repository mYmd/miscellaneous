//index_sequence header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_INDEX_SEQUENCE_INCLUDED
#define MMYYMMDD_INDEX_SEQUENCE_INCLUDED

#include <type_traits>
#include <utility>

namespace mymd  {
    template <typename T, T...values>
    struct integEr_sequence  {
        using value_type = T;
        static constexpr std::size_t size() { return sizeof...(values); }
    };

    template<std::size_t...indices>
    using indEx_sequence = integEr_sequence<std::size_t, indices...>;
    //------------------------------------------------
    template <typename T1, typename T2>	struct index_cat;

    template <std::size_t... indices1, std::size_t... indices2>
    struct index_cat<indEx_sequence<indices1...>, indEx_sequence<indices2...>>
    { using type = indEx_sequence<indices1..., indices2...>; };

    template <std::size_t N, typename T>    struct index_shift;

    template <std::size_t N, std::size_t... indices>
    struct index_shift<N, indEx_sequence<indices...>> { using type = indEx_sequence<N + indices...>; };
    //------------------------------------------------------------------------
    namespace detail_index_range_i  {
        template <std::size_t first, std::size_t len>
        struct index_range_i
        { using type = typename index_shift<first, typename index_range_i<0, len>::type>::type; };

        template <std::size_t len>
        struct index_range_i<0, len>		{
            static constexpr std::size_t h = len/2;
            using type = typename index_cat<typename index_range_i<0, h>::type, typename index_range_i<h, len-h>::type>::type;
        };
        
        template <> struct index_range_i<0, 0>	{ using type = indEx_sequence<>; };
        template <> struct index_range_i<0, 1>	{ using type = indEx_sequence<0>; };
        template <> struct index_range_i<0, 2>	{ using type = indEx_sequence<0, 1>; };
    }
    //+**************************************************************
    template <std::size_t first, std::size_t last>
    using index_range = typename detail_index_range_i::index_range_i<first, (first<last)? last - first: 0>::type;

    template <std::size_t last>
    using make_indEx_sequence = typename detail_index_range_i::index_range_i<0, last>::type;
}

namespace mymd  {
    namespace detail_index_at   {
        
        //  get N_th type of a sequence of types
        template <std::size_t , typename T> struct type_at	{ using type = T; };
        
        template <typename , typename...>	struct pair_t;
        template <std::size_t... num, typename... types>
        struct pair_t<indEx_sequence<num...>, types...>     :   type_at<num, types>... {};
        
        template <std::size_t N>
        struct acceptor	{
            template <typename T>
            static type_at<N, T> upcast(const type_at<N, T>&);
        };
        
        //  get N_th index of a sequence of values
        template <std::size_t, typename> struct att_imple;

        template <std::size_t N, typename T, template <typename U, U...> class value_seq_t, T... values>
        struct att_imple<N, value_seq_t<T, values...>>   {
        private:
            template <T v> struct t_value { static constexpr T value = v; };
            using type_imple = pair_t<make_indEx_sequence<sizeof...(values)>, t_value<values>...>;
        public:
            using type = att_imple<N, value_seq_t<T, values...>>;
            constexpr static T value = decltype(acceptor<N>::upcast(type_imple{}))::type::value;
        };

        template <std::size_t N, template <typename...> class tuple_t, typename... T>
        struct att_imple<N, tuple_t<T...>>    {
        private:
            using type_imple = pair_t<make_indEx_sequence<sizeof...(T)>, T...>;
            using type_0    =  decltype(acceptor<N>::upcast(type_imple{}));
        public:
            using type = typename type_0::type;
        };

        template <std::size_t N>
        struct att_imple<N, void>    {
            using type  =   att_imple<N, void>;
            template <typename T>
            using apply = typename att_imple<N, T>::type;
        };

    }   //namespace detail_index_at	
    
    //+**************************************************************
    // get N_th value of a sequence of values  att<N, value_sequence>::value;    att<N>::apply<value_sequence>::value:
    // get N_th type of a sequence of types    att<N, type_sequence>;   att<N>::apply<type_sequence>;
    template <std::size_t N, typename T = void>
    using att = typename detail_index_at::att_imple<N, T>::type;

    template <typename T>
    struct tat  {
        template <std::size_t N>
        using at = typename detail_index_at::att_imple<N, T>::type;
    };
}

namespace mymd  {
    namespace detail_reverse	{
        template <typename...>	struct reverse_make;
        
        template <template <typename...> class types, typename...result, typename first, typename... tail>
        struct reverse_make<types<result...>, first, tail...> : reverse_make<types<first, result...>, tail...> {};
        
        template <template <typename...> class types, typename...result>
        struct reverse_make<types<result...>>   { using type = types<result...>; };

        template <typename T>	struct reverse_imple;
        template <template <typename...> class types, typename...E>
        struct reverse_imple<types<E...>>   { using type = typename reverse_make<types<>, E...>::type; };
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

        template <template <typename...> class T>
        constexpr std::size_t count2(decltype(count1<T>())) { return count1<T>(); }

        template <template <typename...> class T>
        constexpr std::size_t count2(...)                   { return (std::size_t)-1; }

    }   //detail_count_template_parameters
    
    //関数 count
    template <template <typename...> class T>
    constexpr std::size_t	count_template_parameters()
        { return detail_count_template_parameters::count2<T>(0); }

    //variable templateに対してはどう書けばいいの？
    //template <??????????>
    //constexpr std::size_t    count();
}

#endif
