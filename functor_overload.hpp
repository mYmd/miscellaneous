//functor_overload header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_FUNCTOR_OVERLOAD_INCLUDED
#define MMYYMMDD_FUNCTOR_OVERLOAD_INCLUDED

#include <type_traits>
#include <utility>

namespace mymd {

    namespace detail_fol    {

        template <typename>
        struct noCondition  {
            static const bool value     =       true;
        };

        template <template <typename> class C = noCondition,    bool B = true>
        struct cond	{ };
        //-----------------------------------------------------
        template <typename...> struct packeR { };
        //-----------------------------------------------------
        template <typename T, typename>
        struct arg  {
            using apply     =   std::enable_if<true, T>;
            using applyr    =   apply;
        };

        template <template <typename> class C,      bool B,     typename V>
        struct arg<cond<C, B>, V>  {
            using apply     =   std::enable_if< B == C<V>::value, V   >;
            using applyr    =   std::enable_if< B == C<V>::value, V&& >;
        };
        //-----------------------------------------------------
        struct no_action    {
            constexpr no_action() { }
            template <typename... V>
            void operator ()(V&&...) const {}
        };
        //-----------------------------------------------------
        template <typename F,   typename... A>
        class bolt  {
            F       fn;
            template <typename T, typename U>
            using apply     =    typename arg<T, U>::apply::type;  //workaround for VC
        protected:
            template <typename... V>
            constexpr auto invoke(packeR<V...>, typename arg<A, V>::applyr::type... a) const
                ->decltype(fn(std::declval<apply<A, V>>()...))
            {
                return fn(std::forward<apply<A, V>>(a) ...);
            }
            template <typename... V>
            constexpr auto invoke(packeR<V...>, typename arg<A, V>::applyr::type... a)
                ->decltype(fn(std::declval<apply<A, V>>()...))
            {
                return fn(std::forward<apply<A, V>>(a) ...);
            }
        public:
            constexpr bolt(const F& f) : fn(f)  {  }
        };
        //-----------------------------------------------------
        template <  typename T1,    typename T2 >
        class bolts :   private T1,     private T2  {
            protected:      using T1::invoke;    using T2::invoke;
        public:
            constexpr bolts(const T1& t1, const T2& t2) :   T1(t1),     T2(t2) { }
            template <typename... V>
            constexpr auto operator()(V&&... v) const
                ->decltype(invoke(packeR<V...>{}, std::declval<V>()...))
            {
                return invoke(packeR<V...>{}, std::forward<V>(v)...);
            }
            template <typename... V>
            constexpr auto operator()(V&&... v)
                ->decltype(invoke(packeR<V...>{}, std::declval<V>()...))
            {
                return invoke(packeR<V...>{}, std::forward<V>(v)...);
            }
        };

        template <typename F,   typename... A>
        class bolts<bolt<F, A...>, void> :      private bolt<F, A...>  {
            protected :     using bolt<F, A...>::invoke;
        public:
            constexpr bolts(const F& t) : bolt<F, A...>(t) { }
            template <typename... V>
            constexpr auto operator()(V&&... v) const
                ->decltype(invoke(packeR<V...>{}, std::declval<V>()...))
            {
                return invoke(packeR<V...>{}, std::forward<V>(v)...);
            }
            template <typename... V>
            constexpr auto operator()(V&&... v)
                ->decltype(invoke(packeR<V...>{}, std::declval<V>()...))
            {
                return invoke(packeR<V...>{}, std::forward<V>(v)...);
            }
        };

        template <typename T1, typename T2,     typename U1, typename U2>
        auto operator + (   bolts<T1, T2> const& b,    bolts<U1, U2> const& c   )
            ->bolts<bolts<T1, T2>, bolts<U1, U2>>
        {
            return bolts<bolts<T1, T2>, bolts<U1, U2>>(b, c);
        }
        //-----------------------------------------------------
        template <typename F, typename... A>
        using bolts_t = 
            bolts<bolt<typename std::conditional<std::is_same<F, void>::value, no_action, F>::type, A...>, void>;

    }   //namespace detail_fol
    //********************************************************************
    //  user interfaces are ,
    //  mymd::overload<A, B, C...>(Fn)  ,  mymd::cond<P>  ,  operator +
    //********************************************************************
    using   detail_fol::cond;         //  condition for type

    template <typename T, typename U>
    using overload_t = detail_fol::bolts<T, U>;

    // overload<char, int>(fun);
    template <typename... A, typename F>
    constexpr auto overload(const F& f)
        ->detail_fol::bolts_t<typename std::decay<F>::type, A...>
    {
        return detail_fol::bolts_t<typename std::decay<F>::type, A...>(f);
    }

    // overload<std::is_poineter, std::is_integral>(fun);
    template <template <typename> class... C, typename F>
    constexpr auto overload(const F& f)
        ->detail_fol::bolts_t<typename std::decay<F>::type, cond<C, true>...>
    {
        return detail_fol::bolts_t<typename std::decay<F>::type, cond<C, true>...>(f);
    }

    // no action
    // overload<char, int>();
    template <typename... A>
    constexpr auto overload()
        ->detail_fol::bolts_t<void, A...>
    {
        return detail_fol::bolts_t<void, A...>(detail_fol::no_action{});
    }

    // no action
    // overload<std::is_poineter, std::is_integral>();
    template <template <typename> class... C>
    constexpr auto overload()
        ->detail_fol::bolts_t<void, cond<C, true>...>
    {
        return detail_fol::bolts_t<void, cond<C, true>...>(detail_fol::no_action{});
    }

}   //namespace mymd

#endif

/*
#if defined MMYYMMDD_MBIND_INCLUDED
namespace mymd {
    namespace detail_fol    {
        template <template <typename...> class M, typename... binder, typename V>
        struct arg<mymd::mbind<M, binder...>, V>  {
            using mb = typename mymd::mbind<M, binder...>::template apply<V>;
            using apply  = std::enable_if<mb::value, V>;
            using applyr = std::enable_if<mb::value, V&&>;
        };
    }
}
#endif
*/
