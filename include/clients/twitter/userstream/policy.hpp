#ifndef YUTOPP_TWITTER_POLICY_HPP
#define YUTOPP_TWITTER_POLICY_HPP

#include "../../oauth/core.hpp"
#include "../../basic/core.hpp"
#include "request.hpp"


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
		namespace twitter
		{
			namespace policy
			{
				namespace fusion = boost::fusion;

				template<class AuthType = oauth::core<oauth::version::_1_0> >
				class rest_api
				{
				public:
					typedef AuthType		auth_type;
					typedef std::string		string_type;
				
					static string_type get_domain()
					{
						static const string_type domain = "api.twitter.com";
						return domain;
					}

					static string_type get_path()
					{
						static const string_type path = "/1/";
						return path;
					}

					static string_type get_user_agent()
					{
						static const string_type user_agent = "OAuthLibCpp";
						return user_agent;
					}

					//
					template<typename T>
					static string_type get_api_name()
					{
						using namespace api;
						static const address_type addr(
							fusion::make_pair<statuses::home_timeline>					( "statuses/home_timeline" ),
							fusion::make_pair<statuses::show>							( "statuses/show" ),
							fusion::make_pair<statuses::update>							( "statuses/update" ),
							fusion::make_pair<statuses::friends>						( "statuses/friends" ),

							fusion::make_pair<geo::reverse_geocode>						( "geo/reverse_geocode" ),
												
							fusion::make_pair<account::update_delivery_device>			( "account/update_delivery_device" ),
							fusion::make_pair<account::update_profile_colors>			( "account/update_profile_colors" ),
							fusion::make_pair<account::update_profile_image>			( "account/update_profile_image" ),
							fusion::make_pair<account::update_profile_background_image>	( "account/update_profile_background_image" ),
							fusion::make_pair<account::rate_limit_status>				( "account/rate_limit_status" ),
							fusion::make_pair<account::update_profile>					( "account/update_profile" )
							);

						return fusion::at_key<T>( addr );
					}

					template<typename T>
					static string_type get_api_method()
					{
						return ::webapp::clients::api::method_name<T>::value();
					}

				private:
					typedef fusion::map<
						fusion::pair<api::statuses::home_timeline,						string_type>,
						fusion::pair<api::statuses::show,								string_type>,
						fusion::pair<api::statuses::update,								string_type>,
						fusion::pair<api::statuses::friends,							string_type>,

						fusion::pair<api::geo::reverse_geocode,							string_type>,

						fusion::pair<api::account::update_delivery_device,				string_type>,
						fusion::pair<api::account::update_profile_colors,				string_type>,
						fusion::pair<api::account::update_profile_image,				string_type>,
						fusion::pair<api::account::update_profile_background_image,		string_type>,
						fusion::pair<api::account::rate_limit_status,					string_type>,
						fusion::pair<api::account::update_profile,						string_type>
					> address_type;
				};








				class userstream
				{
				public:
					typedef /*oauth::core<>/*/basic::core/**/		auth_type;
					typedef std::string		string_type;



					static string_type get_domain()
					{
						static const string_type domain = "userstream.twitter.com";
						return domain;
					}

					static string_type get_path()
					{
						static const string_type path = "/2/";
						return path;
					}

					static string_type get_user_agent()
					{
						static const string_type user_agent = "OauthLibCpp";
						return user_agent;
					}




					/*string_type getApiName( const int& id ) const
					{

						return apiInfo_.at( id ).name;
					}

					string_type getApiMethod( const int& id ) const
					{
						return apiInfo_.at( id ).method;
					}*/

				private:



				};



				// Streaming API用のポリシー
				// 2011/3/19 現在まだBasic認証の様で。
				class streaming
				{
				public:
					typedef basic::core		auth_type;
					typedef std::string		string_type;



					static string_type get_domain()
					{
						static const string_type domain = "stream.twitter.com";
						return domain;
					}

					static string_type get_path()
					{
						static const string_type path = "/1/";
						return path;
					}

					static string_type get_user_agent()
					{
						static const string_type user_agent = "OauthLibCpp";	//feel free to change its name.
						return user_agent;
					}




					/*string_type getApiName( const int& id ) const
					{

						return apiInfo_.at( id ).name;
					}

					string_type getApiMethod( const int& id ) const
					{
						return apiInfo_.at( id ).method;
					}*/

				private:



				};

			} // - policy
		} // - twitter
	} // - clients
} // - webapp



#endif /*YUTOPP_TWITTER_POLICY_HPP*/
