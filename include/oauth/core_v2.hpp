//
// core_v2.hpp
// ~~~~~~~~~~~
//

#ifndef YUTOPP_OAUTH_CORE_V2_HPP
#define YUTOPP_OAUTH_CORE_V2_HPP

#include "version.hpp"
#include "../util.hpp"

#include <string>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>

namespace webapp
{
	namespace oauth
	{
		//認証データ
		class key_v2
		{
			typedef std::string oauth_text;

		public:
			key_v2() {}

			key_v2( const oauth_text& client_id, const oauth_text& client_secret )
				: client_id_( client_id )
				, client_secret_( client_secret )
			{}

			void set_client_id( const oauth_text& client_id )
			{
				client_id_ = client_id;
			}
			void set_client_secret( const oauth_text& client_secret )
			{
				client_secret_ = client_secret;
			}
			const oauth_text& set_access_token( const oauth_text& access_token )
			{
				access_token_ = access_token;
			}

			const oauth_text& get_client_id() const
			{
				return client_id_;
			}
			const oauth_text& get_client_secret() const
			{
				return client_secret_;
			}
			const oauth_text& get_access_token() const
			{
				return access_token_;
			}

			bool is_complete() const
			{
				return !( client_id_.empty() || client_secret_.empty() || access_token_.empty() );
			}

		private:
			oauth_text client_id_, client_secret_;
			oauth_text access_token_;
		};

		//本体
		template<>
		class core<version::_2_0>
		{
		public:
			typedef std::string							string_type;
			typedef std::map<string_type, string_type>	param_type;

			typedef key_v2								key_type;

			typedef version::_2_0						version_type;

			//
			core() {}

			core( const key_type& user_key )
				: key_( user_key )
			{}

			//バージョン
			const string_type& get_version() const
			{
				return version_type::get();
			}

			string_type make_authorize_token( const string_type& response_type, const string_type& redirect_uri, param_type&& extension = param_type() ) const
			{
				//
				extension.insert( param_type::value_type( "response_type",	response_type ) );
				extension.insert( param_type::value_type( "client_id",		key_.get_client_id() ) );
				extension.insert( param_type::value_type( "redirect_uri",	redirect_uri ) );
				
				return generate_body_helper( encode_parameter( extension ) );
			}

			string_type make_request_for_access_token( const string_type& grant_type, const string_type& p1, const string_type& p2, param_type& extension = param_type() ) const
			{
				//grant_typeによって求められるパラメータが異なるので処理分け
				if ( grant_type == "authorization_code" ) {
					extension.insert( param_type::value_type( "code",				p1 ) );
					extension.insert( param_type::value_type( "redirect_uri",		p2 ) );

				} else if ( grant_type == "password" ) {
					extension.insert( param_type::value_type( "username",			p1 ) );
					extension.insert( param_type::value_type( "password",			p2 ) );

				} else if ( grant_type == "assertion" ) {
					extension.insert( param_type::value_type( "assertion_type",		p1 ) );
					extension.insert( param_type::value_type( "assertion",			p2 ) );

				} else if ( grant_type == "refresh_token" ) {
					extension.insert( param_type::value_type( "refresh_token",		p1 ) );
				} else {
					//
				}
				extension.insert( param_type::value_type( "grant_type",		grant_type ) );
				extension.insert( param_type::value_type( "client_id",		key_.get_client_id() ) );
				extension.insert( param_type::value_type( "client_secret",	key_.get_client_secret() ) );

				return generate_body_helper( encode_parameter( extension ) );
			}

			//ヘッダの生成
			string_type make_header(...) const
			{
				return "OAuth " + key_.get_access_token();
			}

			//OAuthトークンを設定
			void set_token( const string_type& access_token )
			{
				key_.set_access_token( access_token );
			}

			//
			void set_key( const key_type& k ) { key_ = k; }
			const key_type& get_key() const { return key_; }

		private:
			key_type key_;
		};


		namespace container
		{
			namespace v2
			{
				class access_return
				{
				public:
					typedef std::string							string_type;
					typedef std::map<string_type, string_type>	param_type;

					access_return( const param_type& param, const bool error_flag )
						: value_( param )
						, is_error_( error_flag )
					{}

					const boost::optional<string_type> get_value( const string_type& index )
					{
						const param_type::const_iterator it = value_.find( index );

						if ( it != value_.end() ) {
							return (*it).second;
						} else {
							return boost::none;
						}
					}

					operator bool() const
					{
						return !is_error_;
					}

				private:
					param_type value_;
					bool is_error_;
				};
			}
		}

	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_CORE_V2_HPP*/
