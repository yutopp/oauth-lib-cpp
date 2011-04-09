//
// crypt.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_CRYPT_HPP
#define YUTOPP_CRYPT_HPP

#include <string>
#include <utility>

#include <openssl/hmac.h>
#include <openssl/sha.h>

namespace webapp
{
	namespace crypt
	{
		// HMAC_SHA1
		template<class Ch = char>
		class hmac_sha1
		{	
			typedef std::basic_string<Ch>			string_type;
		public:
			typedef std::pair<string_type, size_t>	return_type;

			return_type operator()( const string_type& key, const string_type& base_string ) const
			{
				size_t			md_len = 0;
				string_type		md( SHA_DIGEST_LENGTH + 1, '\0' );
		
				HMAC( EVP_sha1(), key.c_str(), key.size(), reinterpret_cast<const unsigned char*>( base_string.c_str() ), base_string.size(), reinterpret_cast<unsigned char*>( &md[0] ), &md_len );

				return return_type( /*string_type( */md/* )*/, md_len );
			}

			const string_type& get_method() const
			{
				static const string_type method = "HMAC-SHA1";
				return method;
			}
		};

		// 全てが未実装のRSA-SHA1
		template<class = void>
		class rsa_sha1
		{};

		// 全てが未実装のPLAINTEXT
		template<class = void>
		class plaintext
		{};
	} // - crypt
} // - webapp

#endif /*YUTOPP_CRYPT_HPP*/