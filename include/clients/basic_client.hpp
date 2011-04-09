#ifndef YUTOPP_BASIC_CLIENT_HPP
#define YUTOPP_BASIC_CLIENT_HPP

#include "basic_api.hpp"

#include "sync_part.hpp"
#include "async_part.hpp"

#include "../protocol.hpp"
#include "../util.hpp"

#include <string>
#include <sstream>
#include <tuple>

#include <boost/format.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace webapp
{
	namespace clients
	{
		using api::param_type;

		namespace detail
		{
			template<class Protocol, class Derived, class Policy>
			struct switch_part
			{
				typedef typename boost::mpl::if_<
					protocol::is_sync_connection<Protocol>,
					detail::sync_part<Derived, Protocol, Policy>,
					detail::async_part<Derived, Protocol, Policy>
				>::type type;
			};
		}

		//
		template<class Derived, class Protocol, class Policy>
		class basic_client
			: public detail::switch_part<Protocol, basic_client<Derived, Protocol, Policy>, Policy>::type
		{
			friend detail::switch_part<Protocol, basic_client<Derived, Protocol, Policy>, Policy>::type;

		public:
			typedef typename Policy::auth_type		auth_type;
			typedef typename auth_type::key_type	auth_key_type;

			typedef typename Policy::string_type	string_type;

			explicit basic_client( const auth_key_type& obj )
				: auth_( obj )
				, sender_( Policy::get_domain() )
			{}

			//
			auth_key_type get_auth_key() const
			{
				return auth_.get_key();
			}

			bool is_complete() const
			{
				return auth_.get_key().is_complete();
			}

		private:
			//application/x-www-form-urlencoded
			template<class T>
			string_type request_cushion( const string_type& ext, const param_type& param, typename boost::enable_if<boost::is_base_of<api::encoded_data, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext );
				return static_cast<Derived&>(*this).send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					auth_.make_header( std::get<0>( address ), std::get<2>( address ), param ),
					static_cast<Derived&>(*this).encoded_body<T>( param ),
					"application/x-www-form-urlencoded"
					);
			}
			//application/atom+xml
			template<class T>
			string_type request_cushion( const string_type& ext, const param_type& param, typename boost::enable_if<boost::is_base_of<api::atom_data, T>>::type* =0 )
			{
				return "";
			}
			//multipart/form-data
			template<class T>
			string_type request_cushion( const string_type& ext, const param_type& param, typename boost::enable_if<boost::is_base_of<api::form_data, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext );
				const string_type boundary = util::make_boundary_string();
				return static_cast<Derived&>(*this).send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					auth_.make_header( std::get<0>( address ), std::get<2>( address ), param_type() ),
					static_cast<Derived&>(*this).form_data_body<T>( boundary, param ),
					"multipart/form-data; boundary=" + boundary
					);
			}
			//
			template<class T>
			string_type request_cushion( const string_type& ext, const param_type& param, typename boost::enable_if<boost::is_base_of<api::oridinal, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext, param );
				return static_cast<Derived&>(*this).send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					static_cast<Derived&>(*this).oridinal_header<T>( std::get<0>( address ), std::get<2>( address ), param ),
					static_cast<Derived&>(*this).oridinal_body<T>( param ),
					static_cast<Derived&>(*this).oridinal_content_type<T>()
					);
			}

			//application/x-www-form-urlencoded
			template<class T, class Handler>
			void request_cushion( const string_type& ext, const param_type& param, Handler& handler, typename boost::enable_if<boost::is_base_of<api::encoded_data, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext );
				static_cast<Derived&>(*this).send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					auth_.make_header( std::get<0>( address ), std::get<2>( address ), param ),
					static_cast<Derived&>(*this).encoded_body<T>( param ),
					"application/x-www-form-urlencoded",
					handler
					);
			}
			//application/atom+xml
			template<class T, class Handler>
			string_type request_cushion( const string_type& ext, const param_type& param, Handler& handler, typename boost::enable_if<boost::is_base_of<api::atom_data, T>>::type* =0 )
			{
				return "";
			}
			//multipart/form-data
			template<class T, class Handler>
			void request_cushion( const string_type& ext, const param_type& param, Handler& handler, typename boost::enable_if<boost::is_base_of<api::form_data, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext );
				const string_type boundary = util::make_boundary_string();
				static_cast<Derived*>(this)->send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					auth_.make_header( std::get<0>( address ), std::get<2>( address ), param_type() ),
					static_cast<Derived&>(*this).form_data_body<T>( boundary, param ),
					"multipart/form-data; boundary=" + boundary,
					handler
					);
			}
			//
			template<class T, class Handler>
			void request_cushion( const string_type& ext, const param_type& param, Handler& handler, typename boost::enable_if<boost::is_base_of<api::oridinal, T>>::type* =0 )
			{
				const auto address = static_cast<Derived&>(*this).get_address<T>( ext, param );
				static_cast<Derived&>(*this).send_request(
					std::get<0>( address ),
					std::get<1>( address ),
					static_cast<Derived&>(*this).oridinal_header<T>( std::get<0>( address ), std::get<2>( address ), param ),
					static_cast<Derived&>(*this).oridinal_body<T>( param ),
					static_cast<Derived&>(*this).oridinal_content_type<T>(),
					handler
					);
			}

			//
			string_type make_auth_header( const string_type& method, const string_type& url, const param_type& param ) const
			{
				return auth_.make_header( method, url, param );
			}

			//
			std::tuple<string_type, string_type, string_type> get_address( const string_type& path_name, const string_type& ext ) const
			{
				const string_type path = path_name + "." + ext;
				const string_type url = sender_.get_protocol() + "://" + Policy::get_domain() + path;

				return std::tuple<string_type, string_type, string_type>( string_type(), path, url );
			}
			template<class T>
			std::tuple<string_type, string_type, string_type> get_address( const string_type& ext ) const
			{
				const string_type method = Policy::get_api_method<T>();
				const string_type path = Policy::get_path() + Policy::get_api_name<T>() + "." + ext;
				const string_type url = sender_.get_protocol() + "://" + Policy::get_domain() + path;

				return std::tuple<string_type, string_type, string_type>( method, path, url );
			}
			template<class T>
			std::tuple<string_type, string_type, string_type> get_address( const string_type& ext, const param_type& param ) const
			{
				const string_type method = Policy::get_api_method<T>();
				const string_type path = Policy::get_path() + static_cast<const Derived&>(*this).oridinal_url<T>( Policy::get_api_name<T>(), param ) + "." + ext;
				const string_type url = sender_.get_protocol() + "://" + Policy::get_domain() + path;

				return std::tuple<string_type, string_type, string_type>( method, path, url );
			}


			// -- 派生クラスで書き換えて拡張できるメンバ関数 ここから
			//
			template<class T>
			string_type encoded_body( const param_type& param ) const
			{
				return generate_body_helper( encode_parameter( param ) );
			}

			//
			template<class T>
			string_type form_data_body( const string_type& boundary, const param_type& param ) const
			{
				return T::generate_body( boundary, param );
			}

			//
			template<class T>
			string_type oridinal_header( const string_type& method, const string_type& url, const param_type& param ) const
			{
				return make_auth_header( method, url, T::generate_header_param( param ) );
			}
			template<class T>
			string_type oridinal_body( const param_type& param ) const
			{
				return T::generate_body( param );
			}
			template<class T>
			string_type oridinal_content_type() const
			{
				return "application/x-www-form-urlencoded";
			}
			template<class T>
			string_type oridinal_url( const string_type& name, const param_type& param ) const
			{
				return T::generate_url( boost::format( name ), param ).str();
			}

			//
			std::stringstream make_request( const string_type& method, const string_type& path, const string_type& auth_header, const string_type& body, const string_type& content_type ) const
			{
				std::stringstream ss;
				ss << method << " " << path << " HTTP/1.1\r\n";
				ss << "Host: " << Policy::get_domain() << "\r\n";
				ss << "Accept-Charset: utf-8\r\n";
				ss << "User-Agent: " << Policy::get_user_agent() << "\r\n";
				ss << "Content-type: " << content_type << "\r\n";
				ss << "Authorization: " << auth_header << "\r\n";
				ss << "Content-Length: " << body.size() << "\r\n";
				ss << "Connection: close\r\n";
				ss << "\r\n";
				ss << body << std::flush;

				std::cout << "-------------------------------" << std::endl;
				std::cout << path << std::endl;
				std::cout << ss.str() << std::endl;
				std::cout << "-------------------------------" << std::endl;

				return ss;
			}
			// -- 派生クラスで書き換えて拡張できるメンバ関数 ここまで

		protected:
			string_type send_request( const string_type& method, const string_type& path, const string_type& auth_header, const string_type& body, const string_type& content_type )
			{
				return sender_( static_cast<Derived&>(*this).make_request( method, path, auth_header, body, content_type ) );
			}

			template<class Handler>
			void send_request( const string_type& method, const string_type& path, const string_type& auth_header, const string_type& body, const string_type& content_type, Handler& handler )
			{
				sender_( static_cast<Derived&>(*this).make_request( method, path, auth_header, body, content_type ), handler );
			}

			auth_type auth_;	// 認証
			Protocol sender_;	// 通信
		};
	} // - clients
} // - webapp

#endif /*YUTOPP_BASIC_CLIENT_HPP*/
