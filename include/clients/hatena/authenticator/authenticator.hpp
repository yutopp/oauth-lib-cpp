#ifndef YUTOPP_HATENA_AUTHENTICATOR_HPP
#define YUTOPP_HATENA_AUTHENTICATOR_HPP

#include "policy.hpp"
#include "../../../oauth/authenticator/1/basic_authenticator.hpp"
#include "../../../protocol.hpp"

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

namespace webapp
{
	namespace clients
	{
		namespace hatena
		{
			//
			template<class Protocol = protocol::https, class Policy = policy::authenticator>
			class authenticator
				: public oauth::basic_authenticator<authenticator<Protocol, Policy>, Protocol, Policy>
			{
				typedef oauth::basic_authenticator<authenticator<Protocol, Policy>, Protocol, Policy>
					base_type;

				friend base_type;

				typedef typename base_type::auth_key_type			auth_key_type;
				typedef typename base_type::string_type				string_type;
				typedef typename base_type::request_return_value	request_return_value;
				typedef typename base_type::option_return_value		option_return_value;

				typedef typename Policy::request_pair				request_pair;
				typedef typename Policy::option_data				option_data;

			public:
				explicit authenticator( const auth_key_type& key )
					: base_type( key )
				{}

			private:
				request_return_value parse_request_response( const string_type& response ) const
				{
					using namespace boost::xpressive;

					const sregex rex =
							"oauth_token=" >> ( s1= +_ ) >> "&"
						>>	"oauth_token_secret=" >> ( s2= +_ ) >> "&"
						>>	"oauth_callback_confirmed=" >> ( s3= +_ );
				
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

					const sregex rex =
							"oauth_token=" >> ( s1= +_ ) >> "&"
						>>	"oauth_token_secret=" >> ( s2= +_ ) >> "&"
						>>	"url_name=" >> ( s3= +_ ) >> "&"
						>>  "display_name=" >> ( s4= +_ );
				
					smatch match;
					if ( regex_search( response, match, rex ) ) {
						set_token( match[1], match[2] );
						return option_data( match[3], match[4] );
					} else {
						return boost::none;
					}
				}
			};
		} // - hatena
	} // - clients
} // - webapp

#endif /*YUTOPP_HATENA_AUTHENTICATOR_HPP*/
