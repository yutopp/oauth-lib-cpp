//
// protocol.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_PROTOCOL_HPP
#define YUTOPP_PROTOCOL_HPP

#include "placeholders.hpp"
#include "error.hpp"

#include <string>
#include <sstream>
#include <tuple>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#ifdef OAUTH_USE_SSL
# include <boost/asio/ssl.hpp>
#endif

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace webapp
{
	namespace protocol
	{
		namespace detail
		{
			class attribute
			{
			public:
				attribute()
					: is_chunked_( false )
				{}

				attribute( bool chunked )
					: is_chunked_( chunked )
				{}

				void set_chunked( bool chunked ) { is_chunked_ = chunked; }
				bool is_chunked() const { return is_chunked_; }

			private:
				bool is_chunked_;
			};

			void throw_exception( const error::value& e )
			{
				throw error::exception() << error::code_info( e.code() ) << error::message_info( e.message() );
			}

			//
			template<class Stream>
			boost::optional<error::value> check_response_code( Stream& ss )
			{
				using namespace error;

				std::string line;
				if ( !getline( ss, line ) )
					return value( error::critical, "failed at getline." );

				if ( line.size() < 12 )
					return value( error::critical, "response is too short." );
			
				//
				const auto protocol = line.substr( 0, 5 ), code = line.substr( 9, 3 );

				//レスポンスがおかしかったら
				if ( protocol != "HTTP/" )
					return value( error::critical, "invalid protocol." );

				//ステータスコードが200以外なら
				if ( code != "200" )
					return value( static_cast<error::error_code>( boost::lexical_cast<int>( code ) ), line );

				return boost::none;
			}
			
			//仮
			template<class Stream>
			boost::optional<error::value> check_header( Stream& ss, attribute& attr )
			{
				std::string line, buf;
				while( getline( ss, line ) ) {
					if ( line == "\r" ) {
						find_attribute( buf, attr );
						return boost::none;
					}

					buf += line + "\n";
				}

				return error::value( error::critical, "invalid header." );
			}

			//仮
			void find_attribute( const std::string& buffer, attribute& attr )
			{
				//
				attr.set_chunked( buffer.find( "Transfer-Encoding: chunked" ) != std::string::npos );

				return;
			}

			//
			template<class Stream>
			std::string inline read_body( Stream& ss )
			{
				std::string line, buf;
				while( getline( ss, line ) )
					buf += line + "\n";

				return buf;
			}

			//
			bool cut_header( std::stringstream& ss )
			{
				const auto buffer = ss.str();
				std::size_t point = 0;
				for(;;) {
					const auto now = buffer.find( "\r\n", point );
					if ( now != std::string::npos ) {
						if ( now - point == 0 ) {
							point = now + 2;
							break;
						}
						point = now + 2;
					} else {
						return false;
					}
				}

				ss.str( "" );
				ss << std::string( buffer.cbegin() + point, buffer.cend() );

				return true;
			}

			//良いか！へんな値を渡すんじゃないぞ！
			boost::optional<std::string> cat_chunk( std::stringstream& ss )
			{	
				const std::string buffer = ss.str();
				ss.str( "" );
				ss.seekp( 0, std::ios::cur );

				//std::cout << "- buffer ----------------" << std::endl;
				//std::cout << buffer << std::endl;
				//std::cout << "-------------------------" << std::endl;

				std::string ret;
				std::size_t cursor = 0;
				for(;;) {
					const auto crlf = buffer.find( "\r\n", cursor );
					if ( crlf != std::string::npos ) {
						const std::size_t chunk = util::hex_to_dec( buffer.substr( cursor, crlf - cursor ) );
						//std::cout << "hex : " << buffer.substr( cursor, crlf - cursor ) << " dec: " << chunk << " last : " << ( buffer.size() - ( crlf + 2 ) ) << std::endl;

						if ( chunk == 0 ) {
							cursor = crlf + 2;

						} else if ( chunk < ( buffer.size() - ( crlf + 2 ) ) ) {
							cursor = crlf + 2;
							ret += std::string( buffer.cbegin() + cursor, buffer.cbegin() + cursor + chunk );
							cursor += chunk;

							//std::cout << "- ret -------------------" << std::endl;
							//std::cout << ret << std::endl;
							//std::cout << "-------------------------" << std::endl;
			
						} else {
							// 余りが出たらストリームに戻す
							ss << std::string( buffer.cbegin() + cursor, buffer.cend() );
							break;
						}
					} else {
						//
						break;
					}
				}
				return ret.empty() ? boost::none : boost::optional<std::string>( ret );
			}
		}

		//
		class basic_protocol
//			: private boost::noncopyable
		{
		public:
			basic_protocol( const std::string& protocol, const std::string& domain )
				: protocol_( protocol )
				, domain_( domain )
			{}

			const std::string& get_protocol() const
			{
				return protocol_;
			}

			const std::string& get_domain() const
			{
				return domain_;
			}

		protected:
			~basic_protocol()
			{}

		private:
			std::string protocol_, domain_;
		};

		class post_impl
		{
		public:
			post_impl( boost::asio::io_service& io, const bool always_call )
				: is_always_call_( always_call )
				, is_checked_response_code_( false )
				, is_parsed_header_( false )
				, resolver_( io )
			{}

			template<class ResolveHandler>
			void resolve( const boost::asio::ip::tcp::resolver::query& query, ResolveHandler& handler )
			{
				resolver_.async_resolve( query, handler );
			}

			template<class Handler>
			void notify_error( const error::value& error, Handler& handler ) const
			{
				handler( "", error );
			}

			template<class Handler>
			bool call( Handler& handler )
			{
				if ( !is_checked_response_code_ ) {
					//戻り値==エラー
					if ( const auto&& error = detail::check_response_code( buffer_ ) ) {
						notify_error( *error, handler );		//エラー通知
						return false;
					}
					is_checked_response_code_ = true;
				}

				if ( !is_parsed_header_ ) {
					// ヘッダ
					if ( is_parsed_header_ = !(bool)detail::check_header( buffer_, attribute_ ) )	//ヘッダのチェックが成功したらtrueになるように細工
						detail::cut_header( buffer_ );
				}

				if ( is_parsed_header_ ) {
					// 
					if ( attribute_.is_chunked() ) {
						//
						if ( const auto&& buffer = detail::cat_chunk( buffer_ ) ) {
							handler( *buffer, error::value() );
						}/* else {
							notify_error( handler, error::value( error::critical, "failed protocol::cat_chunk." ) );
							return false;
						}*/
					} else {
						//
						handler( detail::read_body( buffer_ ), error::value() );
					}
				}

				return true;
			};

			void write_to()
			{
				buffer_.write( boost::asio::buffer_cast<const char*>( response_.data() ), response_.size() );
				response_.consume( response_.size() );
			}

			bool is_always_call() const { return is_always_call_; }
			bool is_checked_response_code() const { return is_checked_response_code_; }
			bool is_parsed_header() const { return is_parsed_header_; }

			const boost::asio::streambuf& get_response() const { return response_; }
			boost::asio::streambuf& get_response() { return response_; }

			const std::stringstream& get_buffer() const { return buffer_; }
			std::stringstream& get_buffer() { return buffer_; }

		private:
			bool is_always_call_, is_checked_response_code_, is_parsed_header_;
			detail::attribute attribute_;

			boost::asio::ip::tcp::resolver resolver_;
			boost::asio::streambuf response_;
			std::stringstream buffer_;
		};

		//タグ
		class sync_connection {};
		class async_connection {};

		//
		template<class T, class = void>
		struct is_sync_connection
			: std::false_type
		{};

		template<class T>
		struct is_sync_connection<T, typename boost::enable_if<boost::is_base_of<sync_connection, T>>::type>
			: std::true_type
		{};


		//HTTP
		// 仮
		class http
			: public basic_protocol
			, public sync_connection
		{
		public:
			explicit http( const std::string& domain )
				: basic_protocol( "http", domain )
			{}

			std::string operator()( std::stringstream&& ss ) const
			{
				boost::asio::ip::tcp::iostream stream( get_domain(), get_protocol() );

				//送信
				stream << ss.str() << std::flush;

				//戻り値==エラー
				if ( const auto&& error = detail::check_response_code( stream ) ) {
					//
					detail::throw_exception( *error );
				}
				
				//ヘッダ
				detail::attribute attr;
				if ( const auto&& error = detail::check_header( stream, attr ) ) {
					//era-
					detail::throw_exception( *error );
				}
				
				// 
				if ( attr.is_chunked() ) {
					//
					ss.str( "" );
					ss << &stream;
					if ( const auto&& ret = detail::cat_chunk( ss ) ) {
						return *ret;
					} else {
						detail::throw_exception( error::value( error::critical, "invalid body." ) );
					}
				} else {
					//
					return detail::read_body( ss );
				}
				return "";	//dummy
			}
		};

		//HTTPこっちは非同期
		class async_http
			: public basic_protocol
			, public async_connection
		{
		public:
			async_http( const std::string& domain )
				: basic_protocol( "http", domain )
				, work_( new boost::asio::io_service::work( io_ ) )
			{
				io_runner_ = boost::thread( boost::bind( &boost::asio::io_service::run, &io_ ) );
			}

			~async_http()
			{
				try {
					work_.reset();
					io_.stop();
					
					io_runner_.join();
					io_runner_.detach();
					io_.reset();
				}
				catch(...) {}
			}

			template<class Handler>
			void operator()( std::stringstream&& ss, Handler& handler, const bool always_call = false )
			{
				//
				typedef post<Handler> post_type;
				const auto post_object = boost::make_shared<post_type>( boost::ref( io_ ), boost::cref( ss.str() ), boost::ref( handler ), always_call );

				const auto query = boost::asio::ip::tcp::resolver::query( get_domain(), get_protocol() );
				post_object->resolve( query );
			}

		private:
			template<class Handler>
			class post
				: public boost::enable_shared_from_this<post<Handler>>
			{
				friend class async_http;

				typedef Handler			handler_type;
				typedef std::string		string_type;

			public:
				post( boost::asio::io_service& io, const string_type& data, const handler_type& handler, const bool is_always_call )
					: socket_( io )
					, pimpl_( new post_impl( io, is_always_call ) )
					, data_( data )
					, handler_( handler )
				{}

			private:
				void write()
				{
					boost::asio::async_write( socket_, boost::asio::buffer( data_.c_str(), data_.size() ), 
						boost::bind( &post::handle_write, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
					);
				}

				void read()
				{
					boost::asio::async_read( socket_, pimpl_->get_response(),
						boost::asio::transfer_at_least( 1 ),
						boost::bind( &post::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
					);
				}

				void resolve( const boost::asio::ip::tcp::resolver::query& query )
				{
					//名前解決
					pimpl_->resolve( query,
						boost::bind( &post::handle_resolve, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::iterator
							)
						);
				}

				void handle_resolve( const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
				{
					using namespace boost::asio::ip;
					if ( !error ) {	
						socket_.async_connect( *endpoint_iterator,
							boost::bind( &post::handle_connect, shared_from_this(),
								boost::asio::placeholders::error, ++endpoint_iterator
								)
							);
					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_resolve : " + error.message() ), handler_ );
					}
				}
		
				void handle_connect( const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
				{
					using namespace boost::asio::ip;

					if ( !error ) {
						//
						write();
					} else if ( endpoint_iterator != tcp::resolver::iterator() ) {
						//
						socket_.close();
						socket_.async_connect( *endpoint_iterator,
							boost::bind( &post::handle_connect, shared_from_this(),
								boost::asio::placeholders::error, ++endpoint_iterator
								)
							);
					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_connect : " + error.message() ), handler_ );
					}
				}

				void handle_write( const boost::system::error_code& error, const std::size_t bytes_transferred )
				{
					if ( !error ) {
						read();
					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_write : " + error.message() ), handler_ );
					}
				}

				void handle_read( const boost::system::error_code& error, const std::size_t bytes_transferred )
				{
					if ( !error ) {
						pimpl_->write_to();

						//コールバックの場合分け
						if ( pimpl_->is_always_call() )
							pimpl_->call( handler_ );

						read();
					} else if ( error == boost::asio::error::eof ) {	
						pimpl_->call( handler_ );

					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_read : " + error.message() ), handler_ );
					}
				}

				boost::asio::ip::tcp::socket socket_;
				
				std::unique_ptr<post_impl> pimpl_;
				string_type data_;
				handler_type handler_;
			};
				
			boost::asio::io_service io_;
			std::unique_ptr<boost::asio::io_service::work> work_;

			boost::thread io_runner_;
		};

/**/
#ifdef OAUTH_USE_SSL

		//https
		// 仮
		class https
			: public basic_protocol
			, public sync_connection
		{
		public:
			https( const std::string& domain )
				: basic_protocol( "https", domain )
				, io_()
				, context_( io_, boost::asio::ssl::context::sslv3_client )
			{}

			std::string operator()( std::stringstream&& sstream )
			{
				boost::system::error_code ec;

				//ストリームを作成
				boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream( io_, context_ );

				boost::asio::ip::tcp::resolver resolver( io_ );
				boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve( boost::asio::ip::tcp::resolver::query( get_domain(), get_protocol() ), ec );
				const boost::asio::ip::tcp::resolver::iterator end;
				if ( ec ) {
					detail::throw_exception( error::value( error::critical, "resolve : " + ec.message() ) );
				}

				ec = boost::asio::error::host_not_found;
				while ( ec && endpoint_iterator != end )
				{
					stream.lowest_layer().close( ec );
					stream.lowest_layer().connect( *endpoint_iterator++, ec );
				}
				if ( ec ) {
					detail::throw_exception( error::value( error::critical, "connect : " + ec.message() ) );
				}

				stream.handshake( boost::asio::ssl::stream_base::client, ec );
				if ( ec ) {
					detail::throw_exception( error::value( error::critical, "handshake : " + ec.message() ) );
				}

				//
				boost::asio::write( stream, boost::asio::buffer( sstream.str().c_str(), sstream.str().size() ), boost::asio::transfer_all(), ec );
				if ( ec ) {
					detail::throw_exception( error::value( error::critical, "write : " + ec.message() ) );
				}

				//
				boost::asio::streambuf response;
				sstream.str( "" );
				while( boost::asio::read( stream, response, boost::asio::transfer_at_least( 1 ), ec ) ) {
					sstream << &response;
				}

				//
				if ( const auto&& error = detail::check_response_code( sstream ) ) {
					//detail::throw_exception( *error );
				}

				//ヘッダ
				detail::attribute attr;
				if ( const auto&& error = detail::check_header( sstream, attr ) ) {
					//era-
					detail::throw_exception( *error );
				}
				detail::cut_header( sstream );

				// 
				if ( attr.is_chunked() ) {
					//
					if ( const auto&& ret = detail::cat_chunk( sstream ) ) {
						return *ret;
					} else {
						detail::throw_exception( error::value( error::critical, "invalid body." ) );
					}
				} else {
					//
					return detail::read_body( sstream );
				}
				return "";	//dummy
			}

		private:
			boost::asio::io_service io_;
			boost::asio::ssl::context context_;
		};

		//HTTPSで非同期通信をするんですの
		class async_https
			: public basic_protocol
			, public async_connection
		{
		public:
			async_https( const std::string& domain )
				: basic_protocol( "https", domain )
				, io_()
				, context_( io_, boost::asio::ssl::context::sslv23_client )
				, work_( new boost::asio::io_service::work( io_ ) )
			{
				io_runner_ = boost::thread( boost::bind( &boost::asio::io_service::run, &io_ ) );
			}

			~async_https()
			{
				try {
					work_.reset();
					io_.stop();

					io_runner_.detach();
					io_runner_.join();
					io_.reset();	
				}
				catch(...) {}
			}

			template<class Handler>
			void operator()( std::stringstream&& ss, Handler& handler, const bool always_call = false )
			{
				//
				typedef post<Handler> post_type;
				const auto post_object = boost::make_shared<post_type>( boost::ref( io_ ), boost::ref( context_ ), boost::cref( ss.str() ), boost::ref( handler ), always_call );

				const auto query = boost::asio::ip::tcp::resolver::query( get_domain(), get_protocol() );
				post_object->resolve( query );
			}

		private:
			template<class Handler>
			class post
				: public boost::enable_shared_from_this<post<Handler>>
			{
				friend class async_https;

				typedef Handler			handler_type;
				typedef std::string		string_type;

			public:
				post( boost::asio::io_service& io, boost::asio::ssl::context& context, const string_type& data, handler_type& handler, const bool is_always_call )
					: stream_( io, context )
					, pimpl_( new post_impl( io, is_always_call ) )
					, data_( data )
					, handler_( handler )
				{}

				/*~post()
				{
					stream_.shutdown();
				}*/

			private:
				void write()
				{
					boost::asio::async_write( stream_, boost::asio::buffer( data_.c_str(), data_.size() ), 
						boost::bind( &post::handle_write, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
					);
				}

				void read()
				{
					boost::asio::async_read( stream_, pimpl_->get_response(),
						boost::asio::transfer_at_least( 1 ),
						boost::bind( &post::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
						)
					);
				}

				void resolve( const boost::asio::ip::tcp::resolver::query& query )
				{
					//名前解決
					pimpl_->resolve( query,
						boost::bind( &post::handle_resolve, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::iterator
							)
						);
				}

				void handle_resolve( const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
				{
					using namespace boost::asio::ip;
					if ( !error ) {
						stream_.lowest_layer().async_connect( *endpoint_iterator,
							boost::bind( &post::handle_connect, shared_from_this(),
								boost::asio::placeholders::error, ++endpoint_iterator
								)
							);
					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_resolve : " + error.message() ), handler_ );
					}
				}

				void handle_connect( const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
				{
					using namespace boost::asio::ip;

					if ( !error ) {
						//
						stream_.async_handshake( boost::asio::ssl::stream_base::client,
							boost::bind( &post::handle_handshake, shared_from_this(),
								boost::asio::placeholders::error
								)
							);
					} else if ( endpoint_iterator != tcp::resolver::iterator() ) {
						//
						stream_.lowest_layer().close();
						stream_.lowest_layer().async_connect( *endpoint_iterator,
							boost::bind( &post::handle_connect, shared_from_this(),
								boost::asio::placeholders::error, ++endpoint_iterator
								)
							);
					} else {
						//
						pimpl_->notify_error( error::value( error::critical, "handle_connect : " + error.message() ), handler_ );
					}
				}

				void handle_handshake( const boost::system::error_code& error )
				{
					if ( !error ) {
						write();
					} else {
						//
						pimpl_->notify_error( error::value( error::critical, "handle_handshake : " + error.message() ), handler_ );
					}
				}


				void handle_write( const boost::system::error_code& error, const std::size_t bytes_transferred )
				{
					if ( !error ) {
						read();
					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_write : " + error.message() ), handler_ );
					}
				}

				void handle_read( const boost::system::error_code& error, const std::size_t bytes_transferred )
				{
					if ( !error ) {
						pimpl_->write_to();

						//コールバックの場合分け
						if ( pimpl_->is_always_call() )
							pimpl_->call( handler_ );

						read();

					} else if ( error == boost::asio::error::eof || error == boost::asio::error::shut_down ) {
						//最後
						pimpl_->call( handler_ );

					} else {
						pimpl_->notify_error( error::value( error::critical, "handle_read : " + error.message() ), handler_ );
					}
				}

				boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;
				std::unique_ptr<post_impl> pimpl_;
				string_type data_;
				handler_type handler_;
			};

			boost::asio::io_service io_;
			boost::asio::ssl::context context_;
			std::unique_ptr<boost::asio::io_service::work> work_;

			boost::thread io_runner_;
		};

#endif

	} // - protocol
} // - webapp

#endif /*YUTOPP_PROTOCOL_HPP*/