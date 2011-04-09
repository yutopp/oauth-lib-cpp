#ifndef YUTOPP_OAUTH_COMMON_AUTHENTICATOR_1_HPP
#define YUTOPP_OAUTH_COMMON_AUTHENTICATOR_1_HPP

#include "../base.hpp"
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
				is_v1<typename Policy::auth_type>
			>::type>
			: public basic_authenticator<common_authenticator<Protocol, Policy>, Protocol, Policy>
		{
			typedef basic_authenticator<common_authenticator<Protocol, Policy>, Protocol, Policy>
				base_type;

			friend base_type;

			typedef typename Policy::request_pair				request_pair;
			typedef typename Policy::option_data				option_data;

		public:
			typedef typename base_type::auth_key_type			auth_key_type;
			typedef typename base_type::string_type				string_type;
			typedef typename base_type::request_return_value	request_return_value;
			typedef typename base_type::option_return_value		option_return_value;

			explicit common_authenticator( const auth_key_type& key )
				: base_type( key )
			{}

		private:
			request_return_value parse_request_response( const string_type& response ) const
			{
				using namespace boost::xpressive;
				using namespace util;

				const sregex rex =
						"oauth_token=" >> ( s1= +_ ) >> "&"
					>>	"oauth_token_secret=" >> ( s2= +_ ) >> "&"
					>>	"oauth_callback_confirmed=true";
			
				smatch match;
				if ( regex_search( response, match, rex ) ) {
					return request_pair( match[1], match[2] );
				} else {
					return boost::none;
				}
			}

			option_return_value parse_access_response( const string_type& response )
			{
				using namespace boost::xpressive;
				using namespace util;

				const sregex rex =
						"oauth_token=" >> ( s1= +_ ) >> "&"
					>>	"oauth_token_secret=" >> ( s2= +_ );
			
				smatch match;
				if ( regex_search( response, match, rex ) ) {
					set_token( match[1], match[2] );
					return option_data();
				} else {
					return boost::none;
				}
			}
		};
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_COMMON_AUTHENTICATOR_1_HPP*/
