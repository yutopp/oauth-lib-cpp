//
// core_v1.hpp
// ~~~~~~~~~~~
//

#ifndef YUTOPP_OAUTH_CORE_V1_HPP
#define YUTOPP_OAUTH_CORE_V1_HPP

#include "version.hpp"
#include "../util.hpp"
#include "../crypt.hpp"

#include <string>
#include <map>

//#include <boost/unordered_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/join.hpp>

namespace webapp
{
	namespace oauth
	{
		//認証データ
		class key
		{
			typedef std::string oauth_text;

		public:
			key() {}

			key( const oauth_text& consumer_key, const oauth_text& consumer_secret )
				: consumer_key_( consumer_key )
				, consumer_secret_( consumer_secret )
			{}

			key( const oauth_text& consumer_key, const oauth_text& consumer_secret, const oauth_text& token, const oauth_text& secret )
				: consumer_key_( consumer_key )
				, consumer_secret_( consumer_secret )
				, token_( token )
				, secret_( secret )
			{}

			void set_consumer_key( const oauth_text& consumer_key )
			{
				consumer_key_ = consumer_key;
			}

			void set_consumer_secret( const oauth_text& consumer_secret )
			{
				consumer_secret_ = consumer_secret;
			}

			void set_token( const oauth_text& token )
			{
				token_ = token;
			}

			void set_secret( const oauth_text& secret )
			{
				secret_ = secret;
			}

			const oauth_text& get_consumer_key() const
			{
				return consumer_key_;
			}

			const oauth_text& get_consumer_secret() const
			{
				return consumer_secret_;
			}

			const oauth_text& get_token() const
			{
				return token_;
			}

			const oauth_text& get_secret() const
			{
				return secret_;
			}

			bool is_complete() const
			{
				return !( consumer_key_.empty() || consumer_secret_.empty() || token_.empty() || secret_.empty() );
			}

		private:
			oauth_text consumer_key_, consumer_secret_;
			oauth_text token_, secret_;

		};

		std::string make_key_pair( const std::string& l, const std::string& r = "" )
		{
			using util::encoding::u8;
			using util::url::encode;
			return encode( u8( l ) ) + "&" + encode( u8( r ) );
		}

		//本体
		template<class Version, class Method = crypt::hmac_sha1<> >
		class core
		{
		public:
			typedef std::string							string_type;
			typedef std::map<string_type, string_type>	param_type;

			typedef key									key_type;
			typedef Version								version_type;

			//
			core() {}

			core( const key_type& user_key )
				: key_( user_key )
			{}

			core( const string_type& consumer_key, const string_type& consumer_secret )
			{
				key_.set_consumer_key( consumer_key );
				key_.set_consumer_secret( consumer_secret );
			}

			//シグネチャの生成方式
			const string_type& get_signature_method() const
			{
				return method_.get_method();
			}

			//タイムスタンプ
			static string_type gen_timestamp()
			{
				return boost::lexical_cast<string_type>( ( boost::get_system_time() - boost::posix_time::time_from_string( "1970/1/1 0:0:0" ) ).total_seconds() );
			}

			//Nonce
			static string_type gen_nonce()
			{
				return boost::lexical_cast<string_type>( util::random<>()() ) + gen_timestamp();
			}

			//バージョン
			const string_type& get_version() const
			{
				return version_type::get();
			}

			//ヘッダの生成
			string_type make_header( const string_type& method, const string_type& url, const param_type& body = param_type() ) const
			{
				//
				const param_type must_params = boost::assign::map_list_of
					( "oauth_nonce",			gen_nonce() )
					( "oauth_signature_method",	method_.get_method() )
					( "oauth_timestamp",		gen_timestamp() )
					( "oauth_consumer_key",		key_.get_consumer_key() )
					( "oauth_token",			key_.get_token() )
					( "oauth_version",			get_version() );

				return make_request_header( must_params, body, method, url );
			}

			//リクエストトークンの要求用のヘッダの生成
			string_type make_header_for_request_token( const string_type& method, const string_type& url, const param_type& extension = param_type() ) const
			{
				//
				const param_type must_params = boost::assign::map_list_of
					( "oauth_callback",			string_type( "oob" ) )
					( "oauth_consumer_key",		key_.get_consumer_key() )
					( "oauth_nonce",			gen_nonce() )
					( "oauth_signature_method",	method_.get_method() )
					( "oauth_timestamp",		gen_timestamp() )
					( "oauth_version",			get_version() );	//OPTIONAL

				return make_request_header(
					boost::join( must_params, extension ),
					param_type(),
					method,
					url,
					make_key_pair( key_.get_consumer_secret() )
					);
			}

			//アクセストークンの要求用のヘッダの生成
			string_type make_header_for_access_token( const string_type& method, const string_type& url, const string_type& pin, const string_type& oauth_token, const string_type& oauth_token_secret ) const
			{
				//
				const param_type must_params = boost::assign::map_list_of
					( "oauth_consumer_key",		key_.get_consumer_key() )
					( "oauth_nonce",			gen_nonce() )
					( "oauth_signature_method",	method_.get_method() )
					( "oauth_token",			oauth_token )
					( "oauth_timestamp",		gen_timestamp() )
					( "oauth_verifier",			pin )
					( "oauth_version",			get_version() );	//OPTIONAL
					

				return make_request_header(
					must_params,
					param_type(),
					method,
					url,
					make_key_pair( key_.get_consumer_secret(), oauth_token_secret )
					);
			}

			//OAuthトークンを設定
			void set_token( const string_type& oauth_token, const string_type& oauth_token_secret )
			{
				key_.set_token( oauth_token );
				key_.set_secret( oauth_token_secret );
			}

			//
			void set_key( const key_type& k ) { key_ = k; }
			const key_type& get_key() const { return key_; }

		private:
			//シグネチャの生成
			template<class T>
			string_type make_signature( const T& param, const string_type& method, const string_type& url, const string_type& gen_key ) const
			{
				using namespace util;

				//パラメータの文字列を作成
				string_type paramators = cat_map( param, "=", "&" );
				assert( !paramators.empty() );
				paramators.erase( paramators.end() - 1 );	//最後のを取り除く

				//
				const std::string base_string( method + "&" + url::encode( url ) + "&" + url::encode( paramators ) );

				//
				const std::string key( gen_key.empty() ?
					make_key_pair( key_.get_consumer_secret(), key_.get_secret() ) : gen_key );

				//
				const typename Method::return_type gen( method_( key, base_string ) );

				return base64::encode( gen.first.c_str(), gen.second );
			}

			//
			template<class T, class U>
			string_type make_request_header( const T& must_param, const U& body, const string_type& method, const string_type& url, const string_type& key = "" ) const
			{
				namespace adaptors = boost::adaptors;

				//
				std::string header = "OAuth ";
				header += cat_map( must_param | adaptors::transformed( util::url::encode_pair ) | adaptors::transformed( trns_.encoder() ), "=\"", "\", " );
				header += "oauth_signature=\"";
				header += util::url::encode( make_signature( encode_parameter( boost::join( must_param, body ) ), method, url, key ) );
				header += "\"";

				return header;
			}

			key_type key_;
			Method method_;
			util::encoding::translator<util::encoding::pair_u8_t, util::encoding::pair_sjis_t> trns_;
		};

	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_CORE_V1_HPP*/
