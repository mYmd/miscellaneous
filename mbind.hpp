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
		//  default converter
        template <typename T>
			struct no_cnv  { using type = T; };
        //-----------------------------------------------
		//  placeholder
		template <template <typename> class...convert>
			struct _X_ {
				template <template <typename> class...a>
				using append = _X_<convert..., a...>;
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
		template <template <typename> class...>	struct apply_conv;
		template <>
		struct apply_conv<>	{	//  workaround for VC
			template <typename V>
			using apply = V;
		};
		template <template <typename> class C>
		struct apply_conv<C>	{
			template <typename V>
			using apply = typename has_type<C<V>>::type;
		};
		template <template <typename> class first, template <typename> class...tail>
		struct apply_conv<first, tail...>	{
			template <typename V>
			using apply = typename has_type<first<typename apply_conv<tail...>::template apply<V>>>::type;
		};
		//-----------------------------------------------
		template <typename T>
			struct if_placeholder            { static const std::size_t value = 0; };
		template <template <typename> class...C>
			struct if_placeholder<_X_<C...>> { static const std::size_t value = 1; };
		//-----------------------------------------------
		template <typename...>	struct count_placeholder;
		template <typename first, typename...tail>
			struct count_placeholder<first, tail...>
			{ static const std::size_t value = if_placeholder<first>::value + count_placeholder<tail...>::value; };
		template <>
			struct count_placeholder<>
			{ static const std::size_t value = 0; };
		//-----------------------------------------------
		template <typename T>
		struct trap_placeholder {
			static const bool value = false;
			template <typename>
			using convert = T;	//not convert
		};

		template <template <typename> class...C>
		struct trap_placeholder<_X_<C...>> {
			static const bool value = true;
			template <typename V>
			using convert = typename apply_conv<C...>::template apply<V>;	//convert
		};
		//-----------------------------------------------
		template <typename...> struct packeR { };
		//-----------------------------------------------
		template <typename, typename, typename = packeR<>> struct replace_placeholder;
		template <typename T1, typename...T2, typename V1, typename... V2, typename...R>
		struct replace_placeholder<packeR<T1, T2...>, packeR<V1, V2...>, packeR<R...>>	{
			using result = packeR<R...>;
			using alt_t  = typename trap_placeholder<T1>::template convert<V1>;
			using base_v = typename std::conditional<trap_placeholder<T1>::value, packeR<V2...>, packeR<V1, V2...>>::type;
			using base_t = replace_placeholder<packeR<T2...>, base_v, packeR<R..., alt_t>>;
			using type = typename base_t::type;
		};
		template <typename V1, typename...V2, typename...R>
		struct replace_placeholder<packeR<>, packeR<V1, V2...>, packeR<R...>>
			{ using type = packeR<R...>; };
		template <typename...T, typename...R>
		struct replace_placeholder<packeR<T...>, packeR<>, packeR<R...>>
			{ using type = packeR<R..., T...>; };
		//-----------------------------------------------
		template <template <typename...> class, typename>	struct m_pack;
		template <template <typename...> class M, typename...V>
		struct m_pack<M, packeR<V...>>
			{ using type = M<V...>; };
		//-----------------------------------------------
		template <typename left, typename right>
		struct mbind_or	{
			using check = typename std::enable_if<left::count_placeholder == right::count_placeholder>::type;
			static const std::size_t count_placeholder = left::count_placeholder;
			template <typename... V>
			struct apply	{
				static constexpr bool value = left::template apply<V...>::value || right::template apply<V...>::value;
			};
		};
		//------------
		template <typename left, typename right>
		struct mbind_and	{
			using check = typename std::enable_if<left::count_placeholder == right::count_placeholder>::type;
			static const std::size_t count_placeholder = left::count_placeholder;
			template <typename... V>
			struct apply	{
				static constexpr bool value = left::template apply<V...>::value && right::template apply<V...>::value;
			};
		};
		//------------
		template <typename T>
		struct mbind_not	{
			static const std::size_t count_placeholder = T::count_placeholder;
			template <typename... V>
			struct apply	{
				static constexpr bool value = !T::template apply<V...>::value;
			};
		};

	}	//detail_mbind

	using detail_mbind::_X_;
	using _x_ = detail_mbind::_X_<detail_mbind::no_cnv>;
	using _xrr_ = detail_mbind::_X_<std::remove_reference>;
	using _xrcv_ = detail_mbind::_X_<std::remove_cv>;
	using _xrcvr_ = detail_mbind::_X_<std::remove_cv, std::remove_reference>;
	using _xdecay_ = detail_mbind::_X_<std::decay>;

	template <template <typename...> class M, typename... binder>
	class mbind	{
		template <typename... V>
			using S = typename detail_mbind::replace_placeholder<detail_mbind::packeR<binder...>, detail_mbind::packeR<V...>>::type;
	public:
		static const std::size_t count_placeholder = detail_mbind::count_placeholder<binder...>::value;
		template <typename... V>
			using apply = typename detail_mbind::m_pack<M, S<V...>>::type;
		template <typename... B>
			using bind = mbind<M, binder..., B...>;
		template <typename... B>
			using rebind = mbind<M, B...>;
		template <template <typename...> class M2>
			using change = mbind<M2, binder...>;
	};
	//-------------------------------------------------
	namespace detail_mbind  {
		template <typename T, typename = packeR<>, std::size_t C = T::count_placeholder>
			struct _x_supply;
		template <typename T, typename... R, std::size_t C>
		struct _x_supply<T, packeR<R...>, C>	{
			using type = typename _x_supply<T, packeR<_x_, R...>, C - 1>::type;
		};
		template <typename T, typename... R>
		struct _x_supply<T, packeR<R...>, 0>	{
			using type = mbind<T::template apply, R...>;
		};
	}	//detail_mbind

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

}

#endif
