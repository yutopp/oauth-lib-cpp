#ifndef YUTOPP_GOOGLE_AUTHENTICATOR_POLICY_HPP
#define YUTOPP_GOOGLE_AUTHENTICATOR_POLICY_HPP

#include "../../../oauth/core.hpp"

#include <string>

namespace webapp
{
	namespace clients
	{
		namespace google
		{
			namespace policy
			{
				class authenticator_v2
				{
				public:
					typedef oauth::core<oauth::version::_2_0>					auth_type;
					typedef std::string											string_type;

					typedef oauth::container::v2::access_return					access_return_value;

					static string_type get_user_agent()
					{
						static const string_type user_agent = "OAuthLibCpp";
						return user_agent;
					}

					static string_type get_domain()
					{
						static const string_type domain = "accounts.google.com";
						return domain;
					}

					static const string_type& authorize_path()
					{
						static const string_type path = "/o/oauth2/auth";
						return path;
					}

					static string_type access_method() { return "POST"; }
					static const string_type& access_path()
					{
						static const string_type path = "/o/oauth2/token";
						return path;
					}
				};
			} // - policy
		} // - google
	} // - clients
} // - webapp

#endif /*YUTOPP_GOOGLE_AUTHENTICATOR_POLICY_HPP*/
