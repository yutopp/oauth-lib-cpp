#ifndef YUTOPP_TWITTER_STREAMING_POLICY_HPP
#define YUTOPP_TWITTER_STREAMING_POLICY_HPP

#include "../../../oauth/core.hpp"
#include "../../../basic/core.hpp"
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

				// Streaming API用のポリシー
				// 2011/3/19 現在まだBasic認証の様で。
				class streaming_api
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

					//
					template<typename T>
					static string_type get_api_name()
					{
						static const address_type addr(
							fusion::make_pair<api::streaming::firehose>	( "statuses/firehose" ),
							fusion::make_pair<api::streaming::sample>	( "statuses/sample" ),
							fusion::make_pair<api::streaming::filter>	( "statuses/filter" ),
							fusion::make_pair<api::streaming::links>	( "statuses/links" ),
							fusion::make_pair<api::streaming::retweet>	( "statuses/retweet" )
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
						fusion::pair<api::streaming::firehose,	string_type>,
						fusion::pair<api::streaming::sample,	string_type>,
						fusion::pair<api::streaming::filter,	string_type>,
						fusion::pair<api::streaming::links,		string_type>,
						fusion::pair<api::streaming::retweet,	string_type>
					> address_type;
				};

			} // - policy
		} // - twitter
	} // - clients
} // - webapp



#endif /*YUTOPP_TWITTER_STREAMING_POLICY_HPP*/
