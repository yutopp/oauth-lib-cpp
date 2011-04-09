#ifndef YUTOPP_CLIENT_DETAIL_SYNC_PART_HPP
#define YUTOPP_CLIENT_DETAIL_SYNC_PART_HPP

#include "basic_api.hpp"
#include "../util.hpp"
#include "../response.hpp"
#include "../error.hpp"

#include <string>
#include <utility>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>

//void request
#define REQUEST_FUNCTION_TEMPLATE(n) \
	template<class T BOOST_PP_ENUM_TRAILING_PARAMS(n, class P)>

#define REQUEST_FUNCTION_ARG( z, n, unused ) \
	BOOST_PP_COMMA_IF(n) BOOST_PP_CAT(BOOST_PP_CAT(const P, n),BOOST_PP_CAT(& p, n))

#define REQUEST_FUNCTION( n, pre, type, sig ) \
	REQUEST_FUNCTION_TEMPLATE(n) \
	pre ## REQUEST_FUNCTION_ ## type ## _ARG_LIST(n,sig) \
	pre ## REQUEST_FUNCTION_ ## type ## _IMPL(n,sig)

//requestを定義
#define SYNC_REQUEST_FUNCTION_COMMON_ARG_LIST( n, unused ) \
	string_type inline request( const std::string& ext BOOST_PP_COMMA_IF(n)BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _) )

#define SYNC_REQUEST_FUNCTION_COMMON_IMPL( n, unused ) \
	{ \
		return static_cast<Derived&>(*this).template request_cushion<T>( ext, T()(BOOST_PP_ENUM_PARAMS(n,p)) ); \
	}

#define SYNC_REQUEST_FUNCTION_COMMON_ARG_LIST_WITH_ERROR( n, unused ) \
	string_type inline request( const std::string& ext BOOST_PP_COMMA_IF(n) BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _), error::value& error )

#define SYNC_REQUEST_FUNCTION_COMMON_IMPL_WITH_ERROR( n, unused ) \
	{ \
		try { \
			return static_cast<Derived&>(*this).template request_cushion<T>( ext, T()(BOOST_PP_ENUM_PARAMS(n,p) ) ); \
		} \
		catch( const boost::exception& e ) \
		{ \
			error = e; \
		} \
		return ""; \
	}

#define SYNC_REQUEST_FUNCTION_COMMON( z, n, unused ) \
	REQUEST_FUNCTION( n, SYNC_, COMMON, _ )

#define DEFINE_SYNC_REQUEST_FUNCTION_COMMON( n ) \
	BOOST_PP_REPEAT( \
		BOOST_PP_ADD( 1, n ), \
		SYNC_REQUEST_FUNCTION_COMMON, \
		_ \
	)


//request_[hogehoge]を定義
#define SYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST(n,sig) \
	response request_ ## sig ## ( BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _) )

#define SYNC_REQUEST_FUNCTION_CUSTOM_IMPL(n,sig) \
	{ \
		return sig ## _tree_helper( request<T>( BOOST_PP_STRINGIZE( sig ), BOOST_PP_ENUM_PARAMS(n,p) ) ); \
	}

#define SYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST_WITH_ERROR(n,sig) \
	response request_ ## sig ## ( BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _) BOOST_PP_COMMA_IF(n) error::value& error )

#define SYNC_REQUEST_FUNCTION_CUSTOM_IMPL_WITH_ERROR(n,sig) \
	{ \
		return sig ## _tree_helper( request<T>( BOOST_PP_STRINGIZE( sig ), BOOST_PP_ENUM_PARAMS(n,p), error ) ); \
	}

#define SYNC_REQUEST_FUNCTION_CUSTOM( z, n, sig ) \
	REQUEST_FUNCTION( n, SYNC_, CUSTOM, sig )

#define DEFINE_SYNC_REQUEST_FUNCTION_CUSTOM( sig, n ) \
	BOOST_PP_REPEAT( \
		BOOST_PP_ADD( 1, n ), \
		SYNC_REQUEST_FUNCTION_CUSTOM, \
		sig \
	)


namespace webapp
{
	namespace clients
	{
		namespace detail
		{
			namespace api = ::webapp::clients::api;
			using api::param_type;

			//コールバックが必須
			template<class Derived, class Protocol, class Policy>
			class sync_part
			{
				typedef typename Policy::string_type	string_type;
			public:

				DEFINE_SYNC_REQUEST_FUNCTION_COMMON( 8 )
				DEFINE_SYNC_REQUEST_FUNCTION_CUSTOM( xml, 8 )

				// -- エラーを受け取る
				string_type raw_encoded_request( const string_type& method, const string_type& name, const string_type& ext, const string_type& body, error::value& error )
				{
					try {
						return raw_encoded_request( method, name, ext, body );
					}
					catch( const error::exception& e ) {
						error = e;
					}
					return "";
				}
				string_type raw_encoded_request( const string_type& method, const string_type& name, const string_type& ext, const param_type& param, error::value& error )
				{
					try {
						return raw_encoded_request( method, name, ext, param );
					}
					catch( const error::exception& e ) {
						error = e;
					}
					return "";
				}
				string_type raw_form_data_request( const string_type& method, const string_type& name, const string_type& ext, const string_type& body, const string_type& boundary, error::value& error )
				{
					try {
						return raw_form_data_request( method, name, ext, body, boundary );
					}
					catch( const error::exception& e ) {
						error = e;
					}
					return "";
				}

				// -- エラーを受け取らない（例外に頼る。）
				string_type raw_encoded_request( const string_type& method, const string_type& name, const string_type& ext, const string_type& body )
				{
					if ( !body.empty() ) {
						//内容を分解する
						const param_type param = parse_parameter_helper( body );

						if ( !param.empty() ) {
							//
							return raw_encoded_request( method, name, ext, param );
						} else {
							throw error::exception() << error::code_info( error::critical ) << error::message_info( "bad parameter." );
							return "";
						}
					} else {
						//
						return raw_request( method, name, ext, param_type() );
					}
				}	
				string_type raw_encoded_request( const string_type& method, const string_type& path, const string_type& ext, const param_type& param )
				{
					const auto address = static_cast<Derived&>(*this).get_address( path, ext );	
					return static_cast<Derived&>(*this).send_request(
						method,
						std::get<1>( address ),
						static_cast<Derived&>(*this).make_auth_header( method, std::get<2>( address ), param ),
						generate_body_helper( encode_parameter( param ) ),
						"application/x-www-form-urlencoded"
						);
				}
				string_type raw_form_data_request( const string_type& method, const string_type& path, const string_type& ext, const string_type& body, const string_type& boundary )
				{
					const auto address = static_cast<Derived&>(*this).get_address( path, ext );	
					return static_cast<Derived&>(*this).send_request(
						method,
						std::get<1>( address ),
						static_cast<Derived&>(*this).make_auth_header( method, std::get<2>( address ), param_type() ),
						body,
						"multipart/form-data; boundary=" + boundary
						);
				}
			};

		} // - detail
	} // - clients
} // - webapp

#undef REQUEST_FUNCTION_TEMPLATE
#undef REQUEST_FUNCTION_ARG
#undef REQUEST_FUNCTION
#undef SYNC_REQUEST_FUNCTION_COMMON_ARG_LIST
#undef SYNC_REQUEST_FUNCTION_COMMON_IMPL
#undef SYNC_REQUEST_FUNCTION_COMMON_ARG_LIST_WITH_ERROR
#undef SYNC_REQUEST_FUNCTION_COMMON_IMPL_WITH_ERROR
#undef SYNC_REQUEST_FUNCTION_COMMON
#undef DEFINE_SYNC_REQUEST_FUNCTION_COMMON
#undef SYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST
#undef SYNC_REQUEST_FUNCTION_CUSTOM_IMPL
#undef SYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST_WITH_ERROR
#undef SYNC_REQUEST_FUNCTION_CUSTOM_IMPL_WITH_ERROR
#undef SYNC_REQUEST_FUNCTION_CUSTOM
#undef DEFINE_SYNC_REQUEST_FUNCTION_CUSTOM

#endif /*YUTOPP_CLIENT_DETAIL_SYNC_PART_HPP*/
