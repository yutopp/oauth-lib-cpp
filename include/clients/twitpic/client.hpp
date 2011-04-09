#ifndef YUTOPP_TWITPIC_CLIENT_HPP
#define YUTOPP_TWITPIC_CLIENT_HPP

#include "../basic_client.hpp"

#include "policy.hpp"
#include "request.hpp"

#include "../twitter.hpp"


namespace webapp
{
	namespace clients
	{
		namespace twitpic
		{
			template<class Protocol = protocol::http, class Policy = policy::v2>
			class client
				: public basic_client<client<Protocol, Policy>, Protocol, Policy>
			{
				friend basic_client<client, Protocol, Policy>;

			public:
				explicit client( const string_type& key )
					: key_( key )
				{}

				client( const auth_key_type& auth_key, const string_type& key )
					: basic_client( auth_key )
					, key_( key )
				{}

				template<class Protocol_, class Policy_>
				client( const twitter::client<Protocol_, Policy_>& twitter, const string_type& key )
					: basic_client( twitter.get_auth_key() )
					, key_( key )
				{}
		
				template<class Protocol_, class Policy_>
				void link_to_twitter( const twitter::client<Protocol_, Policy_>& twitter )
				{
					auth_.set_key( twitter.get_auth_key() );
				}
		
				//
				void set_key( const string_type& key ) { key_ = key; }
				const string_type& get_key() const { return key_; }
		
			private:
				//
				std::stringstream make_request( const string_type& method, const string_type& path, string_type&, const string_type& body, const string_type& content_type ) const
				{
					std::stringstream ss;
					ss << method << " " << path << " HTTP/1.1\r\n";
					ss << "Host: " << Policy::get_domain() << "\r\n";
					ss << "Accept-Charset: utf-8\r\n";
					ss << "User-Agent: " << Policy::get_user_agent() << "\r\n";
					ss << "Content-type: " << content_type << "\r\n";
					ss << "X-Auth-Service-Provider: " << "https://api.twitter.com/1/account/verify_credentials.json" << "\r\n";
					ss << "X-Verify-Credentials-Authorization: " << auth_.make_header( "GET", "https://api.twitter.com/1/account/verify_credentials.json" ) << ", realm=\"" << util::url::encode( "http://api.twitter.com/" ) << "\"" << "\r\n";
					ss << "Content-Length: " << body.size() << "\r\n";
					ss << "Connection: close\r\n";
					ss << "\r\n";
					ss << body << std::flush;

					return ss;
				}

				template<class T>
				string_type custom_parameter( string_type&, string_type&, param_type& ) const
				{
					return "";
				}

				//only oririnal
				template<class T>
				//string_type custom_body( const param_type param )
				string_type custom_body( param_type& param ) const
				{
					param.insert( param_type::value_type( "key", util::u8( key ) ) );
					return generate_body_helper( param );
				}

				template<class T>
				string_type form_data_body( const string_type& boundary, const param_type& param )
				{
					return T::generate_body( boundary, key_, param );
				}

				template<class T>
				string_type custom_url( const string_type& name, param_type& ) const
				{
					return name;
				}

				string_type key_;
			};
		}
	}
}

#endif /*YUTOPP_TWITPIC_CLIENT_HPP*/
