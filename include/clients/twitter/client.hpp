#ifndef YUTOPP_TWITTER_CLIENT_HPP
#define YUTOPP_TWITTER_CLIENT_HPP

#include "../basic_client.hpp"

#include "rest/policy.hpp"
#include "rest/request.hpp"

#include "streaming/policy.hpp"
#include "streaming/request.hpp"

namespace webapp
{
	namespace clients
	{
		namespace twitter
		{
			template<class Protocol = protocol::async_http, class Policy = policy::streaming_api>
			class stream
			{
				typedef Protocol									sender_type;
				typedef std::unique_ptr<sender_type>				sender_pointer;

			public:
				typedef typename Policy::string_type	 			string_type;
				typedef typename Policy::auth_type					auth_type;
				typedef typename auth_type::key_type				auth_key_type;

				stream( const auth_key_type& obj )
					: auth_( obj )
					, running_( false )
				{}

				void stop()
				{
					if ( running_ ) {
						sender_.reset();
						running_ = false;
					}
				}

				template<class T, class Handler>
				bool start( const string_type& ext, Handler& handler )
				{ return setup<T>( ext, T()(), handler ); }
				template<class T, class P0, class Handler>
				bool start( const string_type& ext, const P0& p0, Handler& handler )
				{ return setup<T>( ext, T()( p0 ), handler ); }
				template<class T, class P0, class P1, class Handler>
				bool start( const string_type& ext, const P0& p0, const P1& p1, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1 ), handler ); }
				template<class T, class P0, class P1, class P2, class Handler>
				bool start( const string_type& ext, const P0& p0, const P1& p1, const P2& p2, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1, p2 ), handler ); }
				template<class T, class P0, class P1, class P2, class P3, class Handler>
				bool start( const string_type& ext, const P0& p0, const P1& p1, const P2& p2, const P3& p3, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1, p2, p3 ), handler ); }
				template<class T, class P0, class P1, class P2, class P3, class P4, class Handler>
				bool start( const string_type& ext, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1, p2, p3, p4 ), handler ); }
				template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class Handler>
				bool start( const string_type& ext, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1, p2, p3, p4, p5 ), handler ); }
				template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class Handler>
				bool setup( const string_type& ext, const P0& p0, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6, Handler& handler )
				{ return setup<T>( ext, T()( p0, p1, p2, p3, p4, p5, p6 ), handler ); }

			private:
				template<class T, class Handler>
				bool setup( const string_type& ext, const param_type& param, Handler& handler )
				{
					if ( running_ )
						return false;

					//
					sender_.reset( new sender_type( Policy::get_domain() ) );

					//
					const auto method = Policy::get_api_method<T>();
					const auto path = Policy::get_path() + Policy::get_api_name<T>() + "." + ext;
					const auto url = sender_->get_protocol() + "://" + Policy::get_domain() + path;

					//
					const auto encoded_param = encode_parameter( param );

					//
					const auto auth_header = auth_.make_header( method, url, encoded_param );
					const auto body = generate_body_helper( encoded_param );

					//
					std::stringstream ss;
					ss << method << " " << path << " HTTP/1.1\r\n";
					ss << "Host: " << Policy::get_domain() << "\r\n";
					ss << "Accept-Charset: utf-8\r\n";
					ss << "User-Agent: " << Policy::get_user_agent() << "\r\n";
					ss << "Content-type: application/x-www-form-urlencoded\r\n";
					ss << "Authorization: " << auth_header << "\r\n";
					ss << "Connection: Keep-Alive\r\n";
					ss << "Content-Length: " << body.size() << "\r\n";
					ss << "\r\n";
					ss << body << std::flush;

					(*sender_)( std::move( ss ), handler, true );

					running_ = true;

					return true;
				}


				auth_type auth_;
				bool running_;
				sender_pointer sender_;
			};


			//
			template<class Protocol = protocol::http, class Policy = policy::rest_api<>>
			class client
				: public basic_client<client<Protocol, Policy>, Protocol, Policy>
			{
			public:
				explicit client( const auth_key_type& auth_key )
					: basic_client( auth_key )
				{}
			};
		} // - twitter
	} // - clients
} // - webapp

#endif /*YUTOPP_TWITTER_CLIENT_HPP*/
