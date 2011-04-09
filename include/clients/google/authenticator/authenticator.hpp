#ifndef YUTOPP_GOOGLE_AUTHENTICATOR_HPP
#define YUTOPP_GOOGLE_AUTHENTICATOR_HPP

#include "policy.hpp"
#include "../../../oauth/authenticator/2/basic_authenticator.hpp"
#include "../../../protocol.hpp"
#include "../../../util.hpp"

#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

namespace webapp
{
	namespace clients
	{
		namespace google
		{
			//
			template<class Protocol = protocol::https, class Policy = policy::authenticator_v2>
			class authenticator
				: public oauth::basic_authenticator<authenticator<Protocol, Policy>, Protocol, Policy>
			{
				typedef oauth::basic_authenticator<authenticator<Protocol, Policy>, Protocol, Policy>
					base_type;

				friend base_type;

				typedef typename base_type::auth_key_type			auth_key_type;
				typedef typename base_type::string_type				string_type;
				typedef typename Policy::access_data				access_data;
				typedef typename Policy::access_return_value		access_return_value;
				
			public:
				explicit authenticator( const auth_key_type& key )
					: base_type( key )
				{}

			private:
				access_return_value parse_access_response( const string_type& response )
				{
					using namespace boost::xpressive;

					std::cout << response << std::endl;

					const sregex rex =
							"oauth_token=" >> ( s1= +_ ) >> "&"
						>>	"oauth_token_secret=" >> ( s2= +_ ) >> "&"
						>>	"url_name=" >> ( s3= +_ ) >> "&"
						>>  "display_name=" >> ( s4= +_ );
				
					smatch match;
					if ( regex_search( response, match, rex ) ) {
						//set_token( match[1], match[2] );
						return access_data( match[3], match[4] );
					} else {
						return boost::none;
					}
				}
			};
		} // - google
	} // - clients
} // - webapp

#endif /*YUTOPP_GOOGLE_AUTHENTICATOR_HPP*/
