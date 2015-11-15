//mbind header
//Copyright (c) 2014 mmYYmmdd

#if !defined MMYYMMDD_MBIND_INCLUDED
#define MMYYMMDD_MBIND_INCLUDED

#include <type_traits>
#include <utility>

//  Metaなbind    mbind
namespace mymd  {
	namespace detail_mbind  {
        //-----------------------------------------------
        template <typename...> struct packeR { };
        //-----------------------------------------------
        //  default converter
        template <typename T>
            struct no_cnv  { using type = T; };
        //-----------------------------------------------
        //  placeholder
        template <template <typename> class...convert>
            struct _pX_ {
                template <template <typename> class...a>
                using append = _pX_<convert..., a...>;
            };
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
        template <template <typename> class...> struct type_convert;

        template <>
            struct type_convert<>   {   //  workaround for VC
                template <typename V>
                using apply = V;
            };

        template <template <typename> class C>
            struct type_convert<C>  {
                template <typename V>
                using apply = typename has_type<C<V>>::type;
            };

        template <template <typename> class first, template <typename> class...tail>
            struct type_convert<first, tail...> {
                template <typename V>
                using apply = typename has_type<first<typename type_convert<tail...>::template apply<V>>>::type;
            };
        //-----------------------------------------------
        template <typename T>
            struct if_placeholder            { static const std::size_t value = 0; };

        template <template <typename> class...C>
            struct if_placeholder<_pX_<C...>> { static const std::size_t value = 1; };
        //-----------------------------------------------
        template <typename...>	struct count_placeholders;

        template <typename first, typename...tail>
            struct count_placeholders<first, tail...>
            { static const std::size_t value = if_placeholder<first>::value + count_placeholders<tail...>::value; };

        template <>
            struct count_placeholders<>
            { static const std::size_t value = 0; };

        template <typename...T>
            struct count_placeholders<packeR<T...>>
            { static const std::size_t value = count_placeholders<T...>::value; };

        //-----------------------------------------------
        template <typename T>
            struct trap_placeholder {
                static const bool value = false;
                template <typename>
                using convert = T;	//not convert
            };
            
        template <template <typename> class...C>
            struct trap_placeholder<_pX_<C...>> {
                static const bool value = true;
                template <typename V>
                using convert = typename type_convert<C...>::template apply<V>; //convert
            };
        //-----------------------------------------------
        template <typename, typename, typename = packeR<>> struct employ_placeholder;
        
        template <typename T1, typename...T2, typename V1, typename... V2, typename...R>
            struct employ_placeholder<packeR<T1, T2...>, packeR<V1, V2...>, packeR<R...>>  {
                using result = packeR<R...>;
                using alt_t  = typename trap_placeholder<T1>::template convert<V1>;
                using base_v = typename std::conditional<trap_placeholder<T1>::value, packeR<V2...>, packeR<V1, V2...>>::type;
                using base_t = employ_placeholder<packeR<T2...>, base_v, packeR<R..., alt_t>>;
                using type = typename base_t::type;
            };

        template <typename V1, typename...V2, typename...R>
            struct employ_placeholder<packeR<>, packeR<V1, V2...>, packeR<R...>>
            { using type = packeR<R...>; };

        template <typename...T, typename...R>
            struct employ_placeholder<packeR<T...>, packeR<>, packeR<R...>>
            { using type = packeR<R..., T...>; };
        //-----------------------------------------------
        template <typename left, typename right>
            struct mbind_or {
                static_assert(left::n_placeholders == right::n_placeholders, "mbind_or");
                static const std::size_t n_placeholders = left::n_placeholders;
                template <typename... V>
                struct apply    {
                    static constexpr bool value = left::template apply<V...>::value || right::template apply<V...>::value;
                };
            };
        //------------
        template <typename left, typename right>
            struct mbind_and    {
                static_assert(left::n_placeholders == right::n_placeholders, "mbind_and");
                static const std::size_t n_placeholders = left::n_placeholders;
                template <typename... V>
                struct apply    {
                    static constexpr bool value = left::template apply<V...>::value && right::template apply<V...>::value;
                };
            };
        //------------
        template <typename T>
            struct mbind_not    {
                static const std::size_t n_placeholders = T::n_placeholders;
                template <typename... V>
                struct apply    {
                    static constexpr bool value = !T::template apply<V...>::value;
                };
            };
        //-----------------------------------------------
        template <template <typename...> class, typename>   struct unPack;

        template <template <typename...> class M, typename...V>
            struct unPack<M, packeR<V...>>
            { using type = M<V...>; };

    }   //detail_mbind

    using detail_mbind::_pX_;
    using _x_ = detail_mbind::_pX_<detail_mbind::no_cnv>;
    using _xrr_ = detail_mbind::_pX_<std::remove_reference>;
    using _xrcv_ = detail_mbind::_pX_<std::remove_cv>;
    using _xrcvr_ = detail_mbind::_pX_<std::remove_cv, std::remove_reference>;
    using _xdecay_ = detail_mbind::_pX_<std::decay>;
    
    template <template <typename...> class M, typename... binder>
    class mbind {
        template <typename... V>
        using S = typename detail_mbind::employ_placeholder<detail_mbind::packeR<binder...>, detail_mbind::packeR<V...>>::type;
        template <typename> struct rebind2;
        template <typename... B> struct rebind2<detail_mbind::packeR<B...>>
            {   using type = mbind<M, B...>;    };
    public:
        static const std::size_t n_placeholders = detail_mbind::count_placeholders<binder...>::value;
        template <typename... V>
            using apply = typename std::conditional<
                                            detail_mbind::count_placeholders<S<V...>>::value==0,
                                            typename detail_mbind::unPack<M, S<V...>>::type,
                                            typename rebind2<S<V...>>::type
                                          >::type;
        template <typename... B>
            using bind = mbind<M, binder..., B...>;
        template <typename... B>
            using rebind = mbind<M, B...>;
        template <template <typename...> class M2>
            using change = mbind<M2, binder...>;
    };
    //-------------------------------------------------
    namespace detail_mbind  {
        template <typename T, typename = packeR<>, std::size_t C = T::n_placeholders>   struct _x_supply;

        template <typename T, typename... R, std::size_t C>
            struct _x_supply<T, packeR<R...>, C>	{
                using type = typename _x_supply<T, packeR<_x_, R...>, C-1>::type;
            };

        template <typename T, typename... R>
            struct _x_supply<T, packeR<R...>, 0>	{
                using type = mbind<T::template apply, R...>;
            };
    }   //detail_mbind

    template <template <typename...> class M1, typename... B1, template <typename...> class M2, typename... B2>
    auto operator || (const mbind<M1, B1...>& , const mbind<M2, B2...>& )
        -> typename detail_mbind::_x_supply<typename detail_mbind::mbind_or<mbind<M1, B1...>, mbind<M2, B2...>>>::type
    {
        return typename detail_mbind::_x_supply<typename detail_mbind::mbind_or<mbind<M1, B1...>, mbind<M2, B2...>>>::type{};
    }
    
    template <template <typename...> class M1, typename... B1, template <typename...> class M2, typename... B2>
    auto operator && (const mbind<M1, B1...>& , const mbind<M2, B2...>& )
        -> typename detail_mbind::_x_supply<typename detail_mbind::mbind_and<mbind<M1, B1...>, mbind<M2, B2...>>>::type
    {
        return typename detail_mbind::_x_supply<typename detail_mbind::mbind_and<mbind<M1, B1...>, mbind<M2, B2...>>>::type{};
    }

    template <template <typename...> class M, typename... B>
    auto operator ! (const mbind<M, B...>&)
        -> typename detail_mbind::_x_supply<typename detail_mbind::mbind_not<mbind<M, B...>>>::type
    {
        return typename detail_mbind::_x_supply<typename detail_mbind::mbind_not<mbind<M, B...>>>::type{};
    }

}   // mymd

#endif
