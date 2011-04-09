//
// util.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_OAUTH_UTILITY_HPP
#define YUTOPP_OAUTH_UTILITY_HPP

#include <fstream>
#include <exception>
#include <random>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <boost/range/adaptors.hpp>

#include <boost/thread/thread_time.hpp>

#define FUSION_MAX_VECTOR_SIZE	40
#define FUSION_MAX_MAP_SIZE		40
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
#undef FUSION_MAX_VECTOR_SIZE
#undef FUSION_MAX_MAP_SIZE

#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/buffer.h>

#define __USING_SJIS__
#define __UNICODE_CHAR_SIZE_2__
#define __LITTLE_ENDIAN_COMPUTER__	//
#define __BBL_STRING_DEFAULT__
#include <babel.h>
#include <babel.cpp>

namespace webapp
{
	namespace util
	{
		//適当にランダムな値を返す。
		template<int Min = 0, int Max = 12345678/*magic number*/>
		class random
		{
			typedef int num_type;

		public:
			random()
				: gen_( std::random_device()() )
				, rand_( gen_, std::uniform_int_distribution<num_type>( Min, Max ) )
			{}

			num_type operator()() const	//どうしてもconstつけたい・・・！
			{
				return rand_();
			}

		private:
			std::mt19937 gen_;
			mutable std::variate_generator<
				std::mt19937&, std::uniform_int_distribution<num_type>
			> rand_;
		};

		namespace encoding
		{
			const static struct initializer
			{
				initializer()
				{
					babel::init_babel();
				}
			} init;


			//utf8
			std::string inline u8( const std::string& gen )
			{
				return babel::auto_translate<std::string>( gen, babel::base_encoding::utf8 );
			}

			//sjis
			std::string inline sjis( const std::string& gen )
			{
				return babel::auto_translate<std::string>( gen, babel::base_encoding::sjis );
			}


			struct pair_u8_t
			{
				typedef std::pair<std::string, std::string> result_type;

				result_type operator()( const result_type& gen ) const
				{
					return std::make_pair( u8( gen.first ), u8( gen.second ) );
				}
			};

			struct pair_sjis_t
			{
				typedef std::pair<std::string, std::string> result_type;

				result_type operator()( const result_type& gen ) const
				{
					return std::make_pair( sjis( gen.first ), sjis( gen.second ) );
				}
			};


			template<class Encoder, class Decoder>
			struct translator
			{
				Encoder encoder() const { return Encoder(); }
				Decoder decoder() const { return Decoder(); }
			};
		}

		

		int hex_to_dec( const std::string& str )
		{
			static const std::string table( "0123456789ABCDEF" );
			int ret = 0;

			const auto last = str.cend();
			for( auto it = str.cbegin(); it < last; ++it ) {
				ret = ret * 16 + table.find( std::toupper( *it ) );
			}
			return ret;
		}

		//
		namespace url
		{
			/*
			  参考元：http://d.hatena.ne.jp/ytakano/20081016/urlencode
			*/
			std::string encode( const std::string& str )
			{
				std::string st;

				BOOST_FOREACH( const char c, encoding::u8( str ) ) {
					if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) ||
						 ( c >= '0' && c <= '9' ) ||
						   c == '-' || c == '_' || c == '.' || c == '~' )
					{
						st += c;
					} else {
						st += ( boost::format( "%%%1$02X" ) % ( c & 0xff ) ).str();
					}
				}
	
				return st;
			}

			std::pair<std::string, std::string> encode_pair( const std::pair<std::string, std::string>& p )
			{
				return std::make_pair( encode( p.first ), encode( p.second ) );
			}

			/*
			*/
			std::string decode( const std::string& str )
			{
				static const std::string table( "0123456789ABCDEF" );
				std::string st;
				
				const auto last = str.cend();
				for( auto it = str.cbegin(); it < last; ++it ) {
					if ( (*it) == '%' ) {
						if ( ( it + 3 ) > last )
							break;

						char code = table.find( std::toupper( *++it ) ) * 16;
						code += table.find( std::toupper( *++it ) );

						st += code;
					} else if ( (*it) == '+' ) {
						st += ' ';
					} else {
						st += *it;
					}
				}
				return st;
			}

			std::pair<std::string, std::string> decode_pair( const std::pair<std::string, std::string>& p )
			{
				return std::make_pair( decode( p.first ), decode( p.second ) );
			}
		} // - url

		//
		namespace base64
		{
			/*
			  参考元：http://gazpara.blogspot.com/2010/03/pythoncbase64.html
			*/
			std::string encode( const void* src_data, const size_t src_len )
			{
				BUF_MEM* bptr;
	
				if( src_data == NULL || src_len <= 0 )
					throw std::exception( "base64::incorrect data.", 0 );

				BIO* b64 = BIO_new( BIO_f_base64() );
				if ( !b64 )
					throw std::exception( "base64::failed BIO_new BIO_f_base64.", 1 );
	
				BIO* mem = BIO_new( BIO_s_mem() );
				if ( !mem ) {
					BIO_vfree( b64 );
					throw std::exception( "base64::failed BIO_new BIO_f_mem.", 2 );
				}
	
				BIO* chain = BIO_push( b64, mem );
	
				if ( BIO_write( chain, src_data, src_len ) <= 0 ) {
					BIO_free_all( chain );
					throw std::exception( "base64::failed BIO_write.", 3 );
				}

				if( BIO_flush( chain ) <= 0 ) {
					BIO_free_all( chain );
					throw std::exception( "base64::failed BIO_flush.", 4 );
				}
	
				BIO_get_mem_ptr( chain, &bptr );
				/*if ( src_len < bptr->length ) {
					BIO_free_all( chain );
					throw std::exception( "base64::failed to get pointer.", 4 );
				}*/

				const std::string ret( bptr->data, bptr->length - 1 );

				BIO_free_all( chain );

				return ret;
			}
		} // - base64


		std::string make_boundary_string()
		{
			return "--------------" + boost::lexical_cast<std::string>( util::random<>()() );
		}
	} // - util


	//
	template<class T>
	std::string generate_body_helper( const T& args )
	{
		using namespace util::encoding;
		std::string ret;

		BOOST_FOREACH( const auto& arg, args ) {
			if ( !arg.second.empty() ) {
				ret += arg.first + "=" + arg.second + "&";
			}
		}
		//
		if ( !ret.empty() )
			ret.erase( ret.end() - 1 );

		return ret;
	}

	template<class T>
	std::map<T, T> parse_parameter_helper( const T& body )
	{
		using namespace util::encoding;
		using namespace boost::xpressive;
		std::map<T, T> param;
		
		//文字列をパラメータごとに分解する
		const sregex pair = ( (s1= +_w) >> "=" >> (s2= -+_) )
			[ boost::xpressive::ref(param)[s1] = s2 ];
		const sregex rx = pair >> *( "&" >> pair );
		
		regex_match( body, rx );
		
		return param;
	}

	template<class T>
	std::string cat_map( const T& args, const std::string& f, const std::string& l )
	{
		std::string ret;
		BOOST_FOREACH( const auto& arg, args ) {
			ret += arg.first + f + arg.second + l;
		}

		return ret;
	}

	//ダメダメコード
	//とりあえず空白の要素は飛ばして、keyとvalueをutf8にエンコードしたものを格納したmapを返す。
	template<class T>
	std::map<std::string, std::string> encode_parameter( const T& args )
	{
		using namespace util;
		std::map<std::string, std::string> ret;
		BOOST_FOREACH( const auto& arg, args ) {
			if ( arg.second != "" ) {
				ret.insert( T::value_type( url::encode( encoding::u8( arg.first ) ), url::encode( encoding::u8( arg.second ) ) ) );
			}
		}

		return ret;
	}


	//
	std::vector<unsigned char> load_binary_file( const std::string& file )
	{
		//ファイル読み込み
		std::vector<unsigned char> bin;
		std::ifstream fs( file, std::ios::binary );
		if ( !fs )
			throw std::exception( "file not found." );
		
		unsigned char b;
		while( !fs.eof() ) {  //ファイルの最後まで続ける
			fs.read( reinterpret_cast<char*>( &b ), sizeof( b ) );
			bin.push_back( b );
		}

		return bin;
	}
}

#endif /*YUTOPP_OAUTH_UTILITY_HPP*/
