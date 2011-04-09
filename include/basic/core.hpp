//
// core.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_BASIC_CORE_HPP
#define YUTOPP_BASIC_CORE_HPP

#include "../util.hpp"

#include <string>
#include <map>

namespace webapp
{
	namespace basic
	{
		class key
		{
			typedef std::string basic_text;

		public:
			key() {}

			key( const basic_text& username, const basic_text& password )
				: username_( username )
				, password_( password )
			{}

			void set_username( const basic_text& username )
			{
				username_ = username;
			}

			void set_password( const basic_text& password )
			{
				password_ = password;
			}

			const basic_text& get_username() const
			{
				return username_;
			}

			const basic_text& get_password() const
			{
				return password_;
			}

		private:
			basic_text username_, password_;
		};

		class core
		{
		public:
			typedef std::string										string_type;
			typedef std::map<string_type, string_type>				param_type;

			typedef key												key_type;

			core() {}

			core( const key_type& user_key )
				: key_( user_key )
			{}

			core( const string_type& username, const string_type& password )
			{
				key_.set_username( username );
				key_.set_password( password );
			}

			//ヘッダの生成
			string_type make_header(...) const
			{
				using namespace util;

				//
				const auto gen( key_.get_username() + ":" + key_.get_password() );

				return "Basic " + base64::encode( gen.c_str(), gen.size() );
			}

		private:
			key_type key_;
		};

	} // - basic
} // - webapp

#endif /*YUTOPP_BASIC_CORE_HPP*/
