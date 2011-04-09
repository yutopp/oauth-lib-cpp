#ifndef YUTOPP_OAUTH_BASIC_AUTHENTICATOR_1_HPP
#define YUTOPP_OAUTH_BASIC_AUTHENTICATOR_1_HPP

#include "../base.hpp"
#include "sync_part.hpp"
#include "async_part.hpp"

namespace webapp
{
	namespace oauth
	{
		namespace detail
		{
			namespace v1
			{
				template<class Protocol, class Derived, class Policy>
				struct switch_part
				{
					typedef typename boost::mpl::if_<
						protocol::is_sync_connection<Protocol>,
						sync_part<Derived, Protocol, Policy>,
						async_part<Derived, Protocol, Policy>
					>::type type;
				};
			}
		}

		//現時点では非同期専用・・・
		template<class Derived, class Protocol, class Policy>
		class basic_authenticator<Derived, Protocol, Policy, typename boost::enable_if<
				is_v1<typename Policy::auth_type>
			>::type>
			: public detail::v1::switch_part<Protocol, Derived, Policy>::type
		{
			friend detail::v1::switch_part<Protocol, Derived, Policy>::type;

		public:
			typedef typename Policy::auth_type				auth_type;
			typedef typename core_traits<auth_type>::key	auth_key_type;
			typedef typename Policy::string_type			string_type;
			typedef typename Policy::request_return_value	request_return_value;
			typedef typename Policy::option_return_value	option_return_value;

			basic_authenticator( const auth_key_type& key )
				: auth_( key )
				, sender_( Policy::get_domain() )
			{}

			auth_key_type get_auth_key() const
			{
				return auth_.get_key();
			}

			bool is_complete() const
			{
				return auth_.get_key().is_complete();
			}

			string_type get_authorize_url( const string_type& token ) const
			{
				return sender_.get_protocol() + "://" + Policy::get_domain() + Policy::authorize_path() + "?oauth_token=" + util::url::encode( token );
			}

		protected:
			void set_token( const string_type& oauth_token, const string_type& oauth_token_secret )
			{
				auth_.set_token( oauth_token, oauth_token_secret );
			}

			~basic_authenticator() {}

		private:
			std::tuple<string_type, string_type, string_type> get_request_address( /*const param_type& param*/ ) const
			{
				const string_type method = Policy::request_method();
				const string_type path = Policy::request_path();
				const string_type url = sender_.get_protocol() + "://" + Policy::get_domain() + path;

				return std::tuple<string_type, string_type, string_type>( method, path, url );
			}

			std::tuple<string_type, string_type, string_type> get_access_address( /*const param_type& param*/ ) const
			{
				const string_type method = Policy::access_method();
				const string_type path = Policy::access_path();
				const string_type url = sender_.get_protocol() + "://" + Policy::get_domain() + path;

				return std::tuple<string_type, string_type, string_type>( method, path, url );
			}

			//
			std::stringstream make_request( const string_type& method, const string_type& path, const string_type& auth_header, const string_type& body ) const
			{
				std::stringstream ss;
				ss << method << " " << path << " HTTP/1.1\r\n";
				ss << "Host: " << Policy::get_domain() << "\r\n";
				ss << "Accept-Charset: utf-8\r\n";
				ss << "User-Agent: " << Policy::get_user_agent() << "\r\n";
				ss << "Content-type: application/x-www-form-urlencoded\r\n";
				ss << "Authorization: " << auth_header << "\r\n";
				ss << "Content-Length: " << body.size() << "\r\n";
				ss << "Connection: close\r\n";
				ss << "\r\n";
				ss << body << std::flush;

				return ss;
			}

			request_return_value send_request_token_request( const string_type& body )
			{
				using util::encoding::sjis;
				using util::url::decode;

				const auto address( get_request_address() );
				return static_cast<Derived&>(*this).parse_request_response(
					sjis( decode(
						sender_(
							make_request(
								std::get<0>( address ),
								std::get<1>( address ),
								auth_.make_header_for_request_token(
									std::get<0>( address ),
									std::get<2>( address ),
									encode_parameter( parse_parameter_helper( body ) )
									),
								""
								)
							)
						)));
			}
			template<class Handler>
			void send_request_token_request( const string_type& method, const string_type& path, const string_type& url, const string_type& body, Handler& handler )
			{
				using util::encoding::sjis;
				using util::url::decode;
				//static_cast<Derived&>(*this).parse_request_response()
				//sender_( make_request( method, path, auth_.make_header_for_request_token( method, url ), body ), handler );
			}

			option_return_value send_access_token_request( const string_type pin, const string_type& oauth_token, const string_type& oauth_token_secret )
			{
				using util::encoding::sjis;
				using util::url::decode;

				const auto address( get_access_address() );
				return static_cast<Derived&>(*this).parse_access_response(
					sjis( decode(
						sender_(
							make_request(
								std::get<0>( address ),
								std::get<1>( address ),
								auth_.make_header_for_access_token(
									std::get<0>( address ),
									std::get<2>( address ),
									pin,
									oauth_token,
									oauth_token_secret
									),
								""
								)
							)
						)));
			}
			template<class Handler>
			void send_access_token_request( const string_type& method, const string_type& path, const string_type& url, const string_type pin, const string_type& oauth_token, const string_type& oauth_token_secret, Handler& handler )
			{
				using util::encoding::sjis;
				using util::url::decode;
				//static_cast<Derived&>(*this).parse_access_response();
				//sender_( make_request( method, path, auth_.make_header_for_access_token( method, url, pin, oauth_token, oauth_token_secret ), "" ), handler );
			}

/*			request_return_value parse_request_response( const string_type& ) const
			{
				return request_return_value();
			}
			access_return_value parse_access_response( const string_type& )
			{
				return access_return_value();
			}*/

			auth_type auth_;	// 認証
			Protocol sender_;	// 通信
		};
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_BASIC_AUTHENTICATOR_1_HPP*/
