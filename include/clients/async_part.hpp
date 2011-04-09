#ifndef YUTOPP_CLIENT_DETAIL_ASYNC_PART_HPP
#define YUTOPP_CLIENT_DETAIL_ASYNC_PART_HPP

#include "basic_api.hpp"
#include "../util.hpp"
#include "../response.hpp"
#include "../error.hpp"

#include <string>
#include <utility>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>

//
#define REQUEST_FUNCTION_TEMPLATE(n) \
	template<typename T BOOST_PP_ENUM_TRAILING_PARAMS(n, class P), class Handler>

#define REQUEST_FUNCTION_ARG( z, n, unused ) \
	BOOST_PP_CAT(BOOST_PP_CAT(const P, n),BOOST_PP_CAT(& p, n)) ,

#define REQUEST_FUNCTION( n, pre, type, sig ) \
	REQUEST_FUNCTION_TEMPLATE(n) \
	pre ## REQUEST_FUNCTION_ ## type ## _ARG_LIST(n,sig) \
	pre ## REQUEST_FUNCTION_ ## type ## _IMPL(n,sig)


//requestを定義
#define ASYNC_REQUEST_FUNCTION_COMMON_ARG_LIST( n, unused ) \
	void inline request( const std::string& ext, BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _) Handler& handler )

#define ASYNC_REQUEST_FUNCTION_COMMON_IMPL( n, unused ) \
	{ \
		static_cast<Derived&>(*this).template request_cushion<T>( ext, T()(BOOST_PP_ENUM_PARAMS(n,p) ), boost::bind( handler, _1, _2 ) ); \
	}

#define ASYNC_REQUEST_FUNCTION_COMMON( z, n, unused ) \
	REQUEST_FUNCTION( n, ASYNC_, COMMON, _ )

#define DEFINE_ASYNC_REQUEST_FUNCTION_COMMON( n ) \
	BOOST_PP_REPEAT( \
		BOOST_PP_ADD( 1, n ), \
		ASYNC_REQUEST_FUNCTION_COMMON, \
		_ \
	)


//request_[hogehoge]を定義
#define ASYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST(n,sig) \
	void request_ ## sig ## ( BOOST_PP_REPEAT(n, REQUEST_FUNCTION_ARG, _) Handler& handler )

#define ASYNC_REQUEST_FUNCTION_CUSTOM_IMPL(n,sig) \
	{ \
		static_cast<Derived&>(*this).template request_cushion<T>( BOOST_PP_STRINGIZE( sig ), T()(BOOST_PP_ENUM_PARAMS(n,p)), \
			boost::bind( \
				&make_ ## sig ## _translated_caller<formatted_handler_type, response::data_type>, \
				formatted_handler_type( boost::bind( handler, _1, _2 ) ), \
				_1, _2 \
			) ); \
	}

#define ASYNC_REQUEST_FUNCTION_CUSTOM( z, n, sig ) \
	REQUEST_FUNCTION( n, ASYNC_, CUSTOM, sig )

#define DEFINE_ASYNC_REQUEST_FUNCTION_CUSTOM( sig, n ) \
	BOOST_PP_REPEAT( \
		BOOST_PP_ADD( 1, n ), \
		ASYNC_REQUEST_FUNCTION_CUSTOM, \
		sig \
	)


namespace webapp
{
	namespace clients
	{
		namespace detail
		{
			template<class Handler, class String>
			void make_xml_translated_caller( Handler handler, const String& data, const error::value& error )
			{
				handler( xml_tree_helper( data ), error );
			}

			namespace api = ::webapp::clients::api;
			using api::param_type;

			//コールバックが必須
			template<class Derived, class Protocol, class Policy>
			class async_part
			{
				typedef typename Policy::string_type	string_type;
				typedef boost::function<
					void (const response&, const error::value&)
				> formatted_handler_type;

			public:
				// プリプロセッサ使ってみました(^p^)
				// リクエストの送信(結果は素の文字列)
				DEFINE_ASYNC_REQUEST_FUNCTION_COMMON( 10 )
				// リクエストの送信(結果はrequest型)
				DEFINE_ASYNC_REQUEST_FUNCTION_CUSTOM( xml, 10 )

				//
				template<class Handler>
				void raw_request( const string_type& method, const string_type& name, const string_type& ext, const string_type& body, Handler& handler )
				{
					if ( !body.empty() ) {
						//内容を分解する
						const param_type param = parse_parameter_helper( body );

//						if ( !param.empty() ) {
							//
							raw_request( method, name, ext, param, handler );
//						} else {
//							//
//							boost::bind( handler, _1, _2 )( "", error::value( error::critical, "bad parameter." ) );
//						}
					} else {
						//
						return raw_request( method, name, ext, param_type(), handler );
					}
				}
			
				template<class Handler>
				void raw_request( const string_type& method, const string_type& name, const string_type& ext, const param_type& param, Handler& handler )
				{
					const auto address = static_cast<Derived&>(*this).get_address( name, ext );
					const auto encoded_param( encode_parameter( param ) );		
					static_cast<Derived&>(*this).send_request(
						method,
						std::get<1>( address ),
						static_cast<Derived&>(*this).make_auth_header( method, std::get<2>( address ), encoded_param ),
						generate_body_helper( encoded_param ),
						boost::bind( handler, _1, _2 )
						);
				}

				template<class Handler>
				void raw_request( const string_type& method, const string_type& name, const string_type& ext, const string_type& body, const string_type& boundary, Handler& handler )
				{
					const auto address = static_cast<Derived&>(*this).get_address( name, ext );		
					static_cast<Derived*>(this).send_request(
						method,
						std::get<1>( address ),
						static_cast<Derived&>(*this).make_auth_header( method, std::get<2>( address ), param_type() ),
						body,
						boost::bind( handler, _1, _2 ),
						boundary
						);
				}
			};
		} // - detail
	} // - clients
} // - webapp


#undef REQUEST_FUNCTION_TEMPLATE
#undef REQUEST_FUNCTION_ARG
#undef REQUEST_FUNCTION

#undef ASYNC_REQUEST_FUNCTION_COMMON_ARG_LIST
#undef ASYNC_REQUEST_FUNCTION_COMMON_IMPL
#undef ASYNC_REQUEST_FUNCTION_COMMON
#undef DEFINE_ASYNC_REQUEST_FUNCTION_COMMON

#undef ASYNC_REQUEST_FUNCTION_CUSTOM_ARG_LIST
#undef ASYNC_REQUEST_FUNCTION_CUSTOM_IMPL
#undef ASYNC_REQUEST_FUNCTION_CUSTOM
#undef DEFINE_ASYNC_REQUEST_FUNCTION_CUSTOM

#endif /*YUTOPP_CLIENT_DETAIL_ASYNC_PART_HPP*/
