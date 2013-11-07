#ifndef JAG_UTILITY_TRAITS_HPP_
#define JAG_UTILITY_TRAITS_HPP_

#include <type_traits>
#include <utility>

namespace jag {namespace traits {

namespace detail {
	struct return_t {};

	template<typename L, typename R>
	return_t operator<(L const&, R const&);


	template<typename L, typename R>
	struct is_comparable_impl
		: public std::conditional
		< std::is_same
		< decltype(std::declval<L>() < std::declval<R>()), return_t>::value
		, std::false_type
		, std::true_type>::type
	{
	};

}

template<typename L, typename R>
using is_comparable
= detail::is_comparable_impl<L, R>;

template<typename L, typename R>
using is_comparable_t
= typename is_comparable<L, R>::type;

template<typename L, typename R>
using is_not_comparable
= std::is_same<is_comparable_t<L, R>, std::false_type>;


template<typename L, typename R>
using is_not_comparable_t
= typename is_not_comparable<L, R>::type;




} // end of namespace jag::traits

} // end of namespace jag
#endif // JAG_UTILITY_TRAITS_HPP_

