//
// error.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_ERROR_HPP
#define YUTOPP_ERROR_HPP

#include <string>
#include <boost/exception/all.hpp>

namespace webapp
{
	namespace error
	{
		enum error_code
		{
			ok						= 0,	// エラーではない。
			critical				= 1,	// 深刻なエラー、ってことになってる。

			unauthorized			= 401,	// 
			forbidden				= 403,
			unknown,
			not_acceptable			= 406,
			too_long				= 413,
			range_unacceptable		= 416,
			rate_limited			= 420,	// イカんでしょ！
			server_internal_error	= 500,
			service_overloaded		= 503
		};

		class value
		{
		public:
			typedef std::string message_type;

			value()
				: code_( ok )
			{}

			explicit value( const error_code& code )
				: code_( code )
			{}

			value( const error_code& code, const message_type& message )
				: code_( code ), message_( message )
			{}

			const error_code& code() const { return code_; }
			const message_type& message() const { return message_; }

			operator bool() const
			{
				return code_ != ok;
			}

		private:
			error_code code_;
			message_type message_;
		};


		typedef boost::error_info<struct tag_errno, error_code>				code_info;
		typedef boost::error_info<struct tag_errmsg, value::message_type>	message_info;

		struct exception
			: public boost::exception
		{
			operator value() const
			{
				return value( *boost::get_error_info<code_info>( *this ), *boost::get_error_info<message_info>( *this ) );
			}
		};
	} // - error
} // - webapp

#endif /*YUTOPP_ERROR_HPP*/