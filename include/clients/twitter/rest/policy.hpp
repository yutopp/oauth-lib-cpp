#ifndef YUTOPP_TWITTER_REST_POLICY_HPP
#define YUTOPP_TWITTER_REST_POLICY_HPP

#include "../../../oauth/core.hpp"
#include "request.hpp"


#define FUSION_MAX_VECTOR_SIZE	40
#define FUSION_MAX_MAP_SIZE		40
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/include/join.hpp>

#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/include/zip.hpp>

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
						
						static const address_type addr(
							fusion::make_pair<statuses::home_timeline>					( "statuses/home_timeline" ),
							fusion::make_pair<statuses::show>							( "statuses/show" ),
							fusion::make_pair<statuses::update>							( "statuses/update" ),
							fusion::make_pair<statuses::friends>						( "statuses/friends" ),

							fusion::make_pair<list::create>								( "%1%/lists" ),
							fusion::make_pair<list::update>								( "%1%/lists/%2%" ),
							fusion::make_pair<list::index>								( "%1%/lists" ),
							fusion::make_pair<list::show>								( "%1%/lists/%2%" ),
							fusion::make_pair<list::destroy>							( "%1%/lists/%2%" ),
							fusion::make_pair<list::statuses>							( "%1%/lists/%2%/statuses" ),
							fusion::make_pair<list::memberships>						( "%1%/lists/memberships" ),
							fusion::make_pair<list::subscriptions>						( "%1%/lists/subscriptions" ),
							fusion::make_pair<list::get_member>							( "%1%/%2%/members" ),
							fusion::make_pair<list::add_member>							( "%1%/%2%/members" ),
							fusion::make_pair<list::delete_member>						( "%1%/%2%/members" ),
							fusion::make_pair<list::is_member>							( "%1%/%2%/members/%3%" ),
							fusion::make_pair<list::get_subscribers>					( "%1%/%2%/subscribers" ),
							fusion::make_pair<list::subscribe>							( "%1%/%2%/subscribers" ),
							fusion::make_pair<list::unsubscribe>						( "%1%/%2%/subscribers" ),
							fusion::make_pair<list::is_subscriber>						( "%1%/%2%/subscribers/%3%" ),

							fusion::make_pair<geo::reverse_geocode>						( "geo/reverse_geocode" ),
												
							fusion::make_pair<account::update_delivery_device>			( "account/update_delivery_device" ),
							fusion::make_pair<account::update_profile_colors>			( "account/update_profile_colors" ),
							fusion::make_pair<account::update_profile_image>			( "account/update_profile_image" ),
							fusion::make_pair<account::update_profile_background_image>	( "account/update_profile_background_image" ),
							fusion::make_pair<account::rate_limit_status>				( "account/rate_limit_status" ),
							fusion::make_pair<account::update_profile>					( "account/update_profile" )
							);

						return fusion::at_key<T>( fusion::as_map( addr ) );
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

						fusion::pair<api::list::create,									string_type>,
						fusion::pair<api::list::update,									string_type>,
						fusion::pair<api::list::index,									string_type>,
						fusion::pair<api::list::show,									string_type>,
						fusion::pair<api::list::destroy,								string_type>,
						fusion::pair<api::list::statuses,								string_type>,
						fusion::pair<api::list::memberships,							string_type>,
						fusion::pair<api::list::subscriptions,							string_type>,
						fusion::pair<api::list::get_member,								string_type>,
						fusion::pair<api::list::add_member,								string_type>,
						fusion::pair<api::list::delete_member,							string_type>,
						fusion::pair<api::list::is_member,								string_type>,
						fusion::pair<api::list::get_subscribers,						string_type>,
						fusion::pair<api::list::subscribe,								string_type>,
						fusion::pair<api::list::unsubscribe,							string_type>,
						fusion::pair<api::list::is_subscriber,							string_type>,

						fusion::pair<api::geo::reverse_geocode,							string_type>,

						fusion::pair<api::account::update_delivery_device,				string_type>,
						fusion::pair<api::account::update_profile_colors,				string_type>,
						fusion::pair<api::account::update_profile_image,				string_type>,
						fusion::pair<api::account::update_profile_background_image,		string_type>,
						fusion::pair<api::account::rate_limit_status,					string_type>,
						fusion::pair<api::account::update_profile,						string_type>
					> address_type;
				};

			} // - policy
		} // - twitter
	} // - clients
} // - webapp



#endif /*YUTOPP_TWITTER_REST_POLICY_HPP*/
