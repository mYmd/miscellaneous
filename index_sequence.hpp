//index_sequence header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_INDEX_SEQUENCE_INCLUDED
#define MMYYMMDD_INDEX_SEQUENCE_INCLUDED

#include <type_traits>
#include <utility>

//******************************************************************************
//   common utility
//******************************************************************************
namespace mymd  {
    // rebind template-element      change template-template-type
    template <typename> struct template_template;

    template <template<typename...> class D, typename...T>
    struct template_template<D<T...>> {
        template <typename...U>                     using rebind = D<U...>;
        template <template<typename...> class H>    using change = H<T...>;
    };
}

//*******************************************************************************
//   integEr_sequence(integer_sequence),  indEx_sequence(index_sequence),
//   make_indEx_sequence(make_index_sequence)
//   (will be replaced C++14 standard)
//*******************************************************************************
namespace mymd  {
    template <typename T, T...values>
    struct integEr_sequence  {
        using value_type = T;
        static std::size_t size() { return sizeof...(values); }
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
            static const std::size_t h = len/2;
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

//**************************************************************************
//   at_type    /    clas    /    get N_th value of a sequence of values
//      (depends on make_indEx_sequence)
//**************************************************************************
namespace mymd  {
    namespace detail_index_at   {
        
        //  get N_th type of a sequence of types
        template <std::size_t , typename T> struct type_at	{ using type = T; };
        
        template <typename , typename...>	struct pair_t;
        template <std::size_t... num, typename... types>
        struct pair_t<indEx_sequence<num...>, types...>     :   type_at<num, types>... {};
        
        template <std::size_t N>
        struct upcast {
            template <typename T>
            static type_at<N, T> cast(const type_at<N, T>&);
        };
        
        //  get N_th index of a sequence of values
        template <std::size_t, typename> struct att_imple;

        template <std::size_t N, typename T, template <typename U, U...> class value_seq_t, T... values>
        struct att_imple<N, value_seq_t<T, values...>>   {
        private:
            template <T v> struct t_value { static const T value = v; };
            using type_imple = pair_t<mymd::make_indEx_sequence<sizeof...(values)>, t_value<values>...>;
            //using type_imple = pair_t<std::make_index_sequence<sizeof...(values)>, t_value<values>...>;
        public:
            using type = T;
            const static T value = decltype(upcast<N>::cast(type_imple{}))::type::value;
        };

        template <std::size_t N, template <typename...> class tuple_t, typename... T>
        struct att_imple<N, tuple_t<T...>>    {
        private:
            using type_imple = pair_t<mymd::make_indEx_sequence<sizeof...(T)>, T...>;
            //using type_imple = pair_t<std::make_index_sequence<sizeof...(T)>, T...>;
            using type_0    =  decltype(upcast<N>::cast(type_imple{}));
        public:
            using type = typename type_0::type;
        };

        template <std::size_t N>
        struct att_imple<N, void>    {
            template <typename T>
            using in = att_imple<N, T>;
        };

    }   //namespace detail_index_at	
    
    //+**************************************************************
    // get N_th value of a sequence of values  at_type
    // at_type::at<N>::in<T>,      at_type::in<T>::at<N>
    struct at_type  {
        template <std::size_t N>
        struct at   { template <typename T> using in = typename detail_index_at::template att_imple<N, T>::type; };
        template <typename T>
        struct in   { template <std::size_t N> using at = typename detail_index_at::template att_imple<N, T>::type; };
    };
}

//*******************************************************************************
//   count_template_parameters     /    function template   /  as its name
//*******************************************************************************
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
        /*constexpr*/ std::size_t count2(decltype(count1<T>())) { return count1<T>(); }

        template <template <typename...> class T>
        /*constexpr */std::size_t count2(...)                   { return (std::size_t)-1; }

    }   //detail_count_template_parameters
    
    //関数 count
    template <template <typename...> class T>
    constexpr std::size_t	count_template_parameters()
        { return detail_count_template_parameters::count2<T>(0); }

    //variable templateに対してはどう書けばいいの？
    //template <??????????>
    //constexpr std::size_t    count();
}

//*******************************************************************************
//  find_type   /  class template    /   [[deprecated]]
//      (depends on make_indEx_sequence)
//*******************************************************************************
namespace mymd  {
    namespace detail_find    {
        //a little complicated because of VC++
                template <typename...>     struct allFalse         { static const bool value = false; };
                template <typename...T>    struct allFalse<T*...>  { static const bool value = true;  };
                template <bool...b> struct bool_array     {
                    static const bool all_0 = allFalse<typename std::conditional<b, int, int*>::type...>::value;
                };
                struct set_no_default {};
                template <bool, std::size_t, typename A>  struct type_pair {};
                template <std::size_t i, typename A> A upcast(type_pair<true, i, A>&);

        template <template <typename> class Pr, typename default_t = set_no_default>
        struct find_type_imple     {
            template <typename... elem>
            struct apply0   {
                using b_array = bool_array<Pr<elem>::value...>;
                using i_sequence = mymd::make_indEx_sequence<sizeof...(elem)>;
                //using i_sequence = std::make_index_sequence<sizeof...(elem)>;
                static const bool use_default = b_array::all_0 && !std::is_same<default_t, set_no_default>::value;
                static_assert(!(b_array::all_0 && std::is_same<default_t, set_no_default>::value), "mymd::find_type : no match");
                template <typename, typename...>  struct D;
                template <bool...b, std::size_t...i, typename...T>
                    struct D<bool_array<b...>, indEx_sequence<i...>, T...> :
                        type_pair<use_default, 0, default_t>, type_pair<b, i, T>... {};
                static D<b_array, i_sequence, elem...>& getD();
                using type = decltype(upcast( getD() ));
            };
            //----------------------------------------------
            template <typename Arr> struct Apply0;
            template <template <typename...> class Arr, typename... elem>
            struct Apply0<Arr<elem...>> { using type = typename apply0<elem...>::type; };
        };
    }   //namespace detail_find
    //--------------------------------------------------------------------------------------

    struct find_type    {
        //  by<Pred>::from<Range>;
        //  by<Pred>::in<T...>;
        //  from<Range>::by<Pred>;
        //  in<T...>::by<Pred>;
        template <template <typename> class Pr>
        struct by {
            template <typename D, typename default_t = detail_find::set_no_default>
                using from = typename detail_find::find_type_imple<Pr, default_t>::template Apply0<D>::type;
            template <typename...elem>
                using in = typename detail_find::find_type_imple<Pr>::template apply0<elem...>::type;
        };
        template <typename D, typename default_t = detail_find::set_no_default>
        struct from  {
            template <template <typename> class Pr>
                using by = typename detail_find::find_type_imple<Pr, default_t>::template Apply0<D>::type;
        };
        template <typename...elem>
        struct in  {
            template <template <typename> class Pr>
                using by = typename detail_find::find_type_imple<Pr>::template apply0<elem...>::type;
        };
    };

}

//******************************************************************************
//  half_half   /  class template /  separate a sequence in two(head and tail)
//  (head is not actual the first half)
//      (depends on make_indEx_sequence)
//******************************************************************************
namespace mymd  {
    namespace detail_half_half    {
        template <typename> struct pack {};

        template <std::size_t N, typename...T> struct type_n {
            static const std::size_t len = N;   // != sizeof...(T)
            template <std::size_t NN>    using renumber = type_n<NN, T...>;
        };

        template <typename> struct getTail;

        template <template <typename I, I...> class index, std::size_t...i>
        struct getTail<index<std::size_t, i...>> {
            template <std::size_t> struct void_it { using type = void*; };
            template <typename...W>
            static auto get(typename void_it<i>::type... , pack<W>*...)->type_n<0, W...>;
        };

        template<typename> struct half_half;

        template<std::size_t N, typename...T>
        struct half_half<type_n<N, T...>> {
            using I = mymd::make_indEx_sequence<N/2>;
            //using I = std::make_index_sequence<N/2>;
            using tail_0 = decltype(getTail<I>::get( (pack<T>*)(0)... )   );
            using tail = typename tail_0::template renumber<N - N/2>;
            using head = type_n<N - tail::len, T...>;
        };

        template<typename T0, typename...T>
        struct half_half<type_n<1, T0, T...>> {
            using tail = type_n<0>;
            using head = type_n<1, T0>;
        };

        template<typename...T>
        struct half_half<type_n<0, T...>> {
            using tail = type_n<0>;
            using head = type_n<0>;
        };

        template <typename T, bool>   struct type_B {};

        template <typename, typename> struct cat;

        template <std::size_t N1, typename...T1, std::size_t N2, typename...T2>
        struct cat<type_n<N1, T1...>, type_n<N2, T2...>> { using type = type_n<N1 + N2, T1..., T2...>; };

        template <std::size_t N, typename...T1, typename...T2>
        struct cat<type_n<0, T1...>, type_n<N, T2...>> { using type = type_n<N, T2...>; };

        template <std::size_t N, typename...T1, typename...T2>
        struct cat<type_n<N, T1...>, type_n<0, T2...>> { using type = type_n<N, T1...>; };

        template <typename...T1, typename...T2>
        struct cat<type_n<0, T1...>, type_n<0, T2...>> { using type = type_n<0>; };

    }
}

//******************************************************************************
//  select_by_bool   /  class template /  select elements by boolean array
//      (depends on make_indEx_sequence)
//******************************************************************************
namespace mymd  {
    namespace detail_select_bb    {
        using namespace detail_half_half;

        template <typename T, typename = void>  struct select_by_imple;

        template <std::size_t N, typename...T, bool...B>
        struct select_by_imple<type_n<N, type_B<T, B>...>, typename std::enable_if<(2<=N)>::type> {
            using seq = type_n<N, type_B<T, B>...>;
            using head_0 = select_by_imple<typename half_half<seq>::head>;
            using tail_0 = select_by_imple<typename half_half<seq>::tail>;
            using head = typename head_0::type;
            using tail = typename tail_0::type;
            using type = typename cat<head, tail>::type;
        };

        template <typename T1, typename...R>
        struct select_by_imple<type_n<1, type_B<T1, true>, R...>> {
            using type = type_n<1, type_B<T1, true>>;
        };

        template <typename T1, typename...R>
        struct select_by_imple<type_n<1, type_B<T1, false>, R...>> {
            using type = type_n<0>;
        };

        template <typename...R>
        struct select_by_imple<type_n<0, R...>> {
            using type = type_n<0>;
        };

        template <template <typename...> class, typename> struct change_t;

        template <template <typename...> class Arr, std::size_t N, typename...T, bool...B>
        struct change_t<Arr, type_n<N, type_B<T, B>...>> { using type = Arr<T...>; };

        template <template <typename...> class Arr>
        struct change_t<Arr, type_n<0>> { using type = Arr<>; };

    }   //detail_select_bb

    //**************************************************
    template <typename, typename> class select_by_bool;

    template <template <typename...> class Arr, typename...T, template <bool...> class bool_arr, bool...B>
    class select_by_bool<Arr<T...>, bool_arr<B...>> {
        using type0 = 
            typename detail_select_bb::select_by_imple<
                        detail_half_half::type_n<sizeof...(T), detail_half_half::type_B<T, B>...>
                     >::type;
    public:
        using type = typename detail_select_bb::change_t<Arr, type0>::type;
    };
}

//*****************************************************************************************
//  type_if   /  class  /  select elements by a predicate (depends on select_by_bool)
//*****************************************************************************************
namespace mymd  {
    namespace detail_type_if    {
        template <bool...b>     struct bool_array   {};

        template <typename T, template<typename>class Pr>  struct make_b;
        template <template <typename...> class Ar, typename...T, template <typename> class Pr>
            struct make_b<Ar<T...>, Pr> { using type = bool_array<Pr<T>::value...>; };
    }

    struct type_if    {
        //  select<bool...>::from<Range>;
        //  Select<Arr<bool...>>::from<Range>;
        //  by<pred>::from<Range>;
        //  from<Range>::by<pred>;
        //  from<Range>::select<bool...>;
        //  from<Range>::Select<Arr<bool...>>;
        template <bool...b>
        class select {
            template <typename> struct from_imple;
            template <template <typename...> class Arr, typename...T>
            struct from_imple<Arr<T...>>   {
                using ba = detail_type_if::bool_array<b...>;
                using type = typename select_by_bool<Arr<T...>, ba>::type;
            };
        public:
            template <typename D>
                using from = typename from_imple<D>::type;
        };
        template <typename> struct Select;
        template <template <bool...> class B, bool...b>
        struct Select<B<b...>> {
            template <typename D>
                using from = typename type_if::template select<b...>::template from<D>;
        };
        template <template <typename> class Pr>
        class by {
            template <typename D>
                using ba = typename detail_type_if::template make_b<D, Pr>::type;
        public:
            template <typename D>
                using from = typename type_if::template Select<ba<D>>::template from<D>;
        };
        template <typename D>
        struct from  {
            template <bool...b>
                using select = typename type_if::template select<b...>::template from<D>;
            template <typename B>
                using Select = typename type_if::template Select<B>::template from<D>;
            template <template <typename> class Pr>
                using by = typename type_if::template by<Pr>::template from<D>;
        };
    };

}

//*****************************************************************************
//   reverse_t   /   template alias    /   reverse the order of a sequence
//*****************************************************************************
namespace mymd  {
    namespace detail_reverse    {
        using namespace detail_half_half;

        template <typename T, typename = void>  struct reserve_imple;

        template <std::size_t N, typename...T>
        struct reserve_imple<type_n<N, T...>, typename std::enable_if<(3<=N)>::type> {
            using seq = type_n<N, T...>;
            using head_0 = reserve_imple<typename half_half<seq>::head>;
            using tail_0 = reserve_imple<typename half_half<seq>::tail>;
            using head = typename head_0::type;
            using tail = typename tail_0::type;
            using type = typename cat<tail, head>::type;
        };

        template <typename T, typename U, typename...R>
        struct reserve_imple<type_n<2, T, U, R...>> {
            using type = type_n<2, U, T>;
        };

        template <typename T, typename...R>
        struct reserve_imple<type_n<1, T, R...>> {
            using type = type_n<1, T>;
        };

        template <template <typename...> class, typename> struct change_t;

        template <template <typename...> class Arr, std::size_t N, typename...T>
        struct change_t<Arr, type_n<N, T...>> { using type = Arr<T...>; };
        
        template <typename> struct reverse_t_imple;
        
        template <template <typename...> class Arr, typename...T>
        struct reverse_t_imple<Arr<T...>>  {
            using type0 = typename reserve_imple<type_n<sizeof...(T), T...>>::type;
            using type = typename change_t<Arr, type0>::type;
        };

    }
    // reverse the order of some tuple type  --  tupleなどの型の集合を逆順にする
    template <typename T>
    using reverse_t = typename detail_reverse::template reverse_t_imple<T>::type;
}

#endif
