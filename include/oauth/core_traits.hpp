//
// core_traits.hpp
// ~~~~~~~~~~~~~~~
//

#ifndef YUTOPP_OAUTH_CORE_TRAITS_HPP
#define YUTOPP_OAUTH_CORE_TRAITS_HPP

#include "version.hpp"

#include <boost/type_traits.hpp>

namespace webapp
{
	namespace oauth
	{
		template<class T>
		struct core_traits
		{
			typedef typename T::version_type	version;
			typedef typename T::key_type		key;
		};

		template<class T>
		struct is_v1
		{
			static const bool value =
				boost::is_same<typename core_traits<T>::version, version::_1_0>::value ||
				boost::is_same<typename core_traits<T>::version, version::_1_0a>::value;
		};

		template<class T>
		struct is_v2
		{
			static const bool value =
				boost::is_same<typename core_traits<T>::version, version::_2_0>::value;
		};
	}
}

#endif /*YUTOPP_OAUTH_CORE_TRAITS_HPP*/
