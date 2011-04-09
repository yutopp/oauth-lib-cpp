#ifndef YUTOPP_HATENA_POLICY_HPP
#define YUTOPP_HATENA_POLICY_HPP

#include "../../oauth/core_v1.hpp"
#include "requests.hpp"


#define FUSION_MAX_VECTOR_SIZE	40
#define FUSION_MAX_MAP_SIZE		40
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#undef FUSION_MAX_VECTOR_SIZE
#undef FUSION_MAX_MAP_SIZE

namespace webapp
{
	namespace clients
	{
		namespace hatena
		{
			namespace policy
			{
				namespace fusion = boost::fusion;

				template<class AuthType = oauth::core<oauth::version::_1_0> >
				class api
				{
				public:
					typedef AuthType		auth_type;
					typedef std::string		string_type;
				
					static const string_type& get_domain()
					{
						static const string_type domain = "api.twitter.com";
						return domain;
					}

					static const string_type& get_path()
					{
						static const string_type path = "/1/";
						return path;
					}

					static const string_type& get_user_agent()
					{
						static const string_type user_agent = "OAuthLibCpp";
						return user_agent;
					}

					//
					template<typename T>
					static string_type get_api_name()
					{
						using namespace api;
						
						static const address_type addr();

						return fusion::at_key<T>( fusion::as_map( addr ) );
					}

					template<typename T>
					static string_type get_api_method()
					{
						return ::webapp::clients::api::method_name<T>::value();
					}

				private:
					typedef fusion::map<> address_type;
				};

			} // - policy
		} // - hatena
	} // - clients
} // - webapp



#endif /*YUTOPP_HATENA_POLICY_HPP*/
