#ifndef YUTOPP_OAUTH_COMMON_AUTHENTICATOR_2_HPP
#define YUTOPP_OAUTH_COMMON_AUTHENTICATOR_2_HPP

#include "basic_authenticator.hpp"

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

namespace webapp
{
	namespace oauth
	{
		//汎用認証クラス
		template<class Protocol, class Policy>
		class common_authenticator<Protocol, Policy, typename boost::enable_if<
				is_v2<typename Policy::auth_type>
			>::type>
			: public basic_authenticator<common_authenticator<Protocol, Policy>, Protocol, Policy>
		{
			typedef basic_authenticator<common_authenticator<Protocol, Policy>, Protocol, Policy>
				base_type;

			friend base_type;

		public:
			typedef typename base_type::auth_key_type			auth_key_type;
			typedef typename base_type::string_type				string_type;
			typedef typename base_type::access_return_value		access_return_value;

			explicit common_authenticator( const auth_key_type& key )
				: base_type( key )
			{}

		private:
			access_return_value parse_request_response( const string_type& response ) const
			{
				return access_return_value();
			}
		};
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_COMMON_AUTHENTICATOR_2_HPP*/
