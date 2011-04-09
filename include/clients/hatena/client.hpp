#ifndef YUTOPP_HATENA_CLIENT_HPP
#define YUTOPP_HATENA_CLIENT_HPP

#include "../basic_client.hpp"

#include "policy.hpp"
#include "requests.hpp"


namespace webapp
{
	namespace clients
	{
		namespace hatena
		{

			//
			template<class Protocol = protocol::http, class Policy = policy::rest_api<>>
			class client
				: public basic_client<client<Protocol, Policy>, Protocol, Policy>
			{
				typedef basic_client<client<Protocol, Policy>, Protocol, Policy>	base_type;
				typedef typename base_type::auth_key_type							auth_key_type;

			public:
				explicit client( const auth_key_type& auth_key )
					: base_type( auth_key )
				{}
			};
		} // - hatena
	} // - clients
} // - webapp

#endif /*YUTOPP_HATENA_CLIENT_HPP*/
