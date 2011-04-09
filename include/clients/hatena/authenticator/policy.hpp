#ifndef YUTOPP_HATENA_AUTHENTICATOR_POLICY_HPP
#define YUTOPP_HATENA_AUTHENTICATOR_POLICY_HPP

#include "../../../oauth/core.hpp"

#include <string>
#include <utility>
#include <boost/optional.hpp>

namespace webapp
{
	namespace clients
	{
		namespace hatena
		{
			namespace policy
			{
				class authenticator
				{
				public:
					typedef oauth::core<oauth::version::_1_0>					auth_type;
					typedef std::string											string_type;

					typedef std::pair<string_type, string_type>					request_pair;
					typedef std::pair<string_type, string_type>					option_data;

					typedef boost::optional<request_pair>						request_return_value;
					typedef boost::optional<option_data>						option_return_value;

					static string_type get_user_agent()
					{
						static const string_type user_agent = "OAuthLibCpp";
						return user_agent;
					}

					static string_type get_domain()
					{
						static const string_type domain = "www.hatena.com";
						return domain;
					}

					static string_type request_method() { return "POST"; }
					static const string_type& request_path()
					{
						static const string_type path = "/oauth/initiate";
						return path;
					}

					static const string_type& authorize_path()
					{
						static const string_type path = "/oauth/authorize";
						return path;
					}

					static string_type access_method() { return "POST"; }
					static const string_type& access_path()
					{
						static const string_type path = "/oauth/token";
						return path;
					}
				};
			} // - policy
		} // - hatena
	} // - clients
} // - webapp

#endif /*YUTOPP_HATENA_AUTHENTICATOR_POLICY_HPP*/
