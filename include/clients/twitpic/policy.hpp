#ifndef YUTOPP_TWITPIC_POLICY_HPP
#define YUTOPP_TWITPIC_POLICY_HPP

#include "../../oauth/core.hpp"
#include "request.hpp"

#define FUSION_MAX_VECTOR_SIZE	40
#define FUSION_MAX_MAP_SIZE		40
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#undef FUSION_MAX_VECTOR_SIZE
#undef FUSION_MAX_MAP_SIZE
#define i(n,m) api_info(n,m)

namespace webapp
{
	namespace clients
	{
		namespace twitpic
		{
			namespace policy
			{
				namespace fusion = boost::fusion;

				class v2
				{
				public:
					typedef oauth::core<oauth::version::_1_0>	auth_type;
					typedef std::string		string_type;

					static string_type get_domain()
					{
						static const string_type domain = "api.twitpic.com";
						return domain;
					}

					static string_type get_path()
					{
						static const string_type path = "/2/";
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
							fusion::make_pair<upload>				( "upload" ),

							fusion::make_pair<media::show>			( "media/show" ),

							fusion::make_pair<users::show>			( "users/show" ),

							fusion::make_pair<comments::show>		( "comments/show" ),
							fusion::make_pair<comments::create>		( "comments/create" ),
							fusion::make_pair<comments::delete_>	( "comments/delete" ),

							fusion::make_pair<place::show>			( "place/show" ),

							fusion::make_pair<places::show>			( "places/show" ),

							fusion::make_pair<faces::show>			( "faces/show" ),
							fusion::make_pair<faces::create>		( "faces/create" ),
							fusion::make_pair<faces::edit>			( "faces/edit" ),
							fusion::make_pair<faces::delete_>		( "faces/delete" ),

							fusion::make_pair<events::show>			( "events/show" ),

							fusion::make_pair<event::show>			( "event/show" ),
							fusion::make_pair<event::create>		( "event/create" ),
							fusion::make_pair<event::delete_>		( "event/delete" ),
							fusion::make_pair<event::add>			( "event/add" ),
							fusion::make_pair<event::remove>		( "event/remove" ),

							fusion::make_pair<tags::show>			( "tags/show" ),
							fusion::make_pair<tags::create>			( "tags/create" ),
							fusion::make_pair<tags::delete_>		( "tags/delete" )
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
						fusion::pair<api::upload,				string_type>,

						fusion::pair<api::media::show,			string_type>,

						fusion::pair<api::users::show,			string_type>,

						fusion::pair<api::comments::show,		string_type>,
						fusion::pair<api::comments::create,		string_type>,
						fusion::pair<api::comments::delete_,	string_type>,

						fusion::pair<api::place::show,			string_type>,

						fusion::pair<api::places::show,			string_type>,

						fusion::pair<api::faces::show,			string_type>,
						fusion::pair<api::faces::create,		string_type>,
						fusion::pair<api::faces::edit,			string_type>,
						fusion::pair<api::faces::delete_,		string_type>,

						fusion::pair<api::events::show,			string_type>,

						fusion::pair<api::event::show,			string_type>,
						fusion::pair<api::event::create,		string_type>,
						fusion::pair<api::event::delete_,		string_type>,
						fusion::pair<api::event::add,			string_type>,
						fusion::pair<api::event::remove,		string_type>,

						fusion::pair<api::tags::show,			string_type>,
						fusion::pair<api::tags::create,			string_type>,
						fusion::pair<api::tags::delete_,		string_type>
					> address_type;
				};
			}
		}
	}
}

#undef i

#endif /*YUTOPP_TWITPIC_POLICY_HPP*/
