#ifndef YUTOPP_OAUTH_DETAIL_V1_SYNC_PART_HPP
#define YUTOPP_OAUTH_DETAIL_V1_SYNC_PART_HPP

namespace webapp
{
	namespace oauth
	{
		namespace detail
		{
			namespace v1
			{
				template<class Derived, class Protocol, class Policy>
				class sync_part
				{
					typedef typename Policy::string_type			string_type;
					typedef typename Policy::request_pair			request_pair;
					typedef typename Policy::option_data			option_data;
					typedef typename Policy::request_return_value	request_return_value;
					typedef typename Policy::option_return_value	option_return_value;

				public:
					request_return_value get_request_token( const string_type& body = string_type() )
					{
						return static_cast<Derived&>(*this).send_request_token_request( body );
					}

					option_return_value get_access_token( const string_type& pin, const request_pair& request_token )
					{
						return static_cast<Derived&>(*this).send_access_token_request(
								pin,
								request_token.first,
								request_token.second
								);
					}
				};
			} // - v1
		} // - detail
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_DETAIL_V1_SYNC_PART_HPP*/
