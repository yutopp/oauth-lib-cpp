#ifndef YUTOPP_OAUTH_AUTHENTICATOR_BASE_HPP
#define YUTOPP_OAUTH_AUTHENTICATOR_BASE_HPP

#include "../../protocol.hpp"
#include "../core_traits.hpp"

#include <boost/utility/enable_if.hpp>

namespace webapp
{
	namespace oauth
	{
		//これはなに
		template<class Derived, class Protocol, class Policy, class = void>
		class basic_authenticator;

		template<class Protocol, class Policy, class = void>
		class common_authenticator;
	}
}


#endif /*YUTOPP_OAUTH_AUTHENTICATOR_BASE_HPP*/
