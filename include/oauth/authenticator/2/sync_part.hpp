#ifndef YUTOPP_OAUTH_DETAIL_V2_SYNC_PART_HPP
#define YUTOPP_OAUTH_DETAIL_V2_SYNC_PART_HPP

namespace webapp
{
	namespace oauth
	{
		namespace detail
		{
			namespace v2
			{
				template<class Derived, class Protocol, class Policy>
				class sync_part
				{
					typedef typename Policy::string_type			string_type;
					typedef typename Policy::access_return_value	access_return_value;

				public:
					access_return_value get_access_token( const string_type& grant_type, const string_type& p1 )
					{
						return static_cast<Derived&>(*this).send_access_token_request(
								grant_type,
								p1,
								"",		// 第二パラメータは無し
								""		// オプションも無し
								);
					}

					access_return_value get_access_token( const string_type& grant_type, const string_type& p1, const string_type& p2 )
					{
						return static_cast<Derived&>(*this).send_access_token_request(
								grant_type,
								p1,
								p2,
								"" );
					}

					access_return_value get_access_token( const string_type& grant_type, const string_type& p1, const string_type& p2, const string_type& option )
					{
						return static_cast<Derived&>(*this).send_access_token_request(
								grant_type,
								p1,
								p2,
								option );
					}
				};
			} // - v2
		} // - detail
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_DETAIL_V2_SYNC_PART_HPP*/
