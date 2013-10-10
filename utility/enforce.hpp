#ifndef JAG_UTILITY_ENFORCE_HPP_
#define JAG_UTILITY_ENFORCE_HPP_

#include <boost/exception/all.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/thread.hpp>

#include <type_traits>
#include <tuple>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#define JAG_EXP_ENFORCE(exp, validation) \
	::jag::make_enforce(exp, \
	validation, \
	[] {return std::runtime_error("Failed enforcement"); }, \
	[](boost::exception& e) {e << ::jag::enforce_diag_tag(":Expression '" BOOST_PP_STRINGIZE(exp) "' failed!"); }, \
	__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)


#define JAG_EXP_ENFORCE_MSG(exp, validation, msg) \
	::jag::make_enforce(exp, \
	validation, \
	[] {return std::runtime_error("Failed enforcement"); }, \
	[](boost::exception& e) {\
		e << ::jag::enforce_diag_tag(":Expression '" BOOST_PP_STRINGIZE(exp) "' failed!") << (::jag::enforce_info_tag() << msg); }, \
	__FILE__, __LINE__, BOOST_CURRENT_FUNCTION)

#define JAG_ENFORCE(exp) JAG_EXP_ENFORCE(exp, ::jag::enforce_policy_succeed())
#define JAG_ENFORCE_MSG(exp, msg) JAG_EXP_ENFORCE_MSG(exp, ::jag::enforce_policy_succeed(), msg)

namespace jag
{
	using enforce_diag_tag = boost::error_info < struct enforce_diag_tag_, std::string>;
	struct enforce_info_tag : public boost::error_info < struct enforce_info_tag_, std::string>
	{
		typedef boost::error_info < struct enforce_info_tag_, std::string> base_type;
		enforce_info_tag() : base_type("") {}
		template<typename T>
		enforce_info_tag& operator<<(T&& u)
		{
			std::ostringstream buffer; buffer << value() << u; value() = std::move(buffer.str());
			return *this;
		}
	};

	struct enforce_policy_succeed
	{
		template<typename T>bool operator()(T&& u) { return !!u; }
	};

	struct enforce_policy_throw
	{
		std::runtime_error operator()() { return std::runtime_error("Failed enforcement"); }
		void operator()(boost::exception& u) {}
	};

	
	template<typename T
		, typename ValidatorT = enforce_policy_succeed
		, typename RaiserT = enforce_policy_throw
		, typename PopulatorT = enforce_policy_throw
	>
	auto make_enforce(T&& exp, ValidatorT&& validator = ValidatorT(), RaiserT&& raiser = RaiserT(), PopulatorT&& populator = PopulatorT(),
	const char* file = nullptr, int line = -1, const char* function = nullptr)
	->decltype(std::forward<T>(exp))
	{
		if (!validator(std::forward<T>(exp)))
		{
			auto e = boost::enable_error_info(raiser());
			if (file != nullptr)
				e << boost::throw_file(file);
			if (line != -1)
				e << boost::throw_line(line);
			if (function != nullptr)
				e << boost::throw_function(function);
			populator(e);
			throw e;
		}
		return std::forward<T>(exp);

	}

}


#endif // JAG_UTILITY_ENFORCE_HPP_
