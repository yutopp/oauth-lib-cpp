#ifndef YUTOPP_TWITPIC_REQUEST_HPP
#define YUTOPP_TWITPIC_REQUEST_HPP

#include "../../util.hpp"

#include "../basic_api.hpp"

#include <boost/filesystem/path.hpp>			//ファイルパスの解析
#include <boost/algorithm/string/case_conv.hpp>	//tolower

namespace webapp
{
	namespace clients
	{
		namespace twitpic
		{	
			namespace api
			{
				using namespace ::webapp::clients::api;

				//upload[POST]
				struct upload
					: public form_data, tag_post_method
				{
					param_type operator()( s message, s media ) const
					{
						return boost::assign::map_list_of
								( "message",	util::encoding::u8( message ) )
								( "media",		media );
					}

					//
					static string_type generate_body( s boundary, s key, const param_type& param )
					{
						//パス
						const boost::filesystem::path path = param.at( "media" );
						const auto ext( boost::algorithm::to_lower_copy( path.extension().string() ) );
						//twitpicが対応している画像フォーマット以外ならビデオってことにしちゃおう
						const std::string content_type( ( ( ext == ".png" ) && ( ext == ".gif" ) && ( ext == ".jpg" ) && ( ext == ".jpeg" ) ) ? "image" : "video" );
						
						//バイナリ読み込み
						const auto bin( load_binary_file( path.string() ) );

						//
						std::stringstream ss;

						//キー
						ss << "--" << boundary << "\r\n";
						ss << "Content-Disposition: form-data; name=\"key\"" << "\r\n";
						ss << "\r\n";
						ss << key << "\r\n";

						//メッセージ
						ss << "--" << boundary << "\r\n";
						ss << "Content-Disposition: form-data; name=\"message\"" << "\r\n";
						ss << "\r\n";
						ss << param.at( "message" ) << "\r\n";

						//メディア
						ss << "--" << boundary << "\r\n";
						ss << "Content-Disposition: form-data; name=\"media\"; filename=\"" << util::encoding::u8( path.filename().string() ) << "\"" << "\r\n";
						ss << "Content-Type: " << content_type << "/" << std::string( ++ext.begin(), ext.end() ) << "\r\n";
						ss << "Content-Transfer-Encoding: binary" << "\r\n";
						ss << "Content-Length: " << bin.size() << "\r\n";
						ss << "\r\n";
						ss.write( (char*)&bin[0], bin.size() );
						ss << "\r\n";
						ss << "--" << boundary << "--" << std::flush;

						return ss.str();
					}
				};

				//media
				namespace media
				{
					//media/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s id ) const
						{
							return boost::assign::map_list_of( "id",	id );
						}
					};
				}

				//users
				namespace users
				{
					//users/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s username, s page = "" ) const
						{
							return boost::assign::map_list_of
								( "username",	username )
								( "page",		page );
						}
					};
				}

				//comments
				namespace comments
				{
					//comments/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s media_id, s page ) const
						{
							return boost::assign::map_list_of
								( "media_id",	media_id )
								( "page",		page );
						}
					};

					//comments/create[POST]
					struct create
						: public oridinal, tag_post_method
					{
						param_type operator()( s media_id, s message ) const
						{
							return boost::assign::map_list_of
								( "media_id",	media_id )
								( "message",	message );
						}
					};

					//comments/delete[POST]
					struct delete_	//予約語わろた・・・
						: public oridinal, tag_post_method
					{
						param_type operator()( s comment_id ) const
						{
							return boost::assign::map_list_of( "comment_id",	comment_id );
						}
					};
				}

				//place
				namespace place
				{
					//place/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s id, s user = "" ) const
						{
							return boost::assign::map_list_of
								( "id",			id )
								( "user",		user );
						}
					};
				}

				//places
				namespace places
				{
					//places/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s user ) const
						{
							return boost::assign::map_list_of( "user",	user );
						}
					};
				}

				//faces
				namespace faces
				{
					//faces/show[POST]
					struct show
						: public oridinal, tag_post_method
					{
						param_type operator()( s user ) const
						{
							return boost::assign::map_list_of( "user",	user );
						}
					};
					
					//faces/create[POST]
					struct create
						: public oridinal, tag_post_method
					{
						param_type operator()(
							  s media_id
							, s top_coord
							, s left_coord
							, s name = ""
							, s username = "" ) const
						{
							return boost::assign::map_list_of
								( "media_id",	media_id )
								( "top_coord",	top_coord )
								( "left_coord",	left_coord )
								( "name",		name )
								( "username",	username );
						}
					};

					//faces/edit[POST]
					struct edit
						: public oridinal, tag_post_method
					{
						param_type operator()(
							  s media_id
							, s tag_id
							, s top_coord = ""
							, s left_coord= ""
							, s name = ""
							, s username = "" ) const
						{
							return boost::assign::map_list_of
								( "media_id",	media_id )
								( "tag_id",		tag_id )
								( "top_coord",	top_coord )
								( "left_coord",	left_coord )
								( "name",		name )
								( "username",	username );
						}
					};

					//faces/delete[POST]
					struct delete_
						: public oridinal, tag_post_method
					{
						param_type operator()( s tag_id ) const
						{
							return boost::assign::map_list_of( "tag_id", tag_id );
						}
					};
				}

				//events
				namespace events
				{
					//events/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s user ) const
						{
							return boost::assign::map_list_of( "user",	user );
						}
					};
				}
		
				//event
				namespace event
				{
					//event/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s id ) const
						{
							return boost::assign::map_list_of( "id",	id );
						}
					};

					//event/create[POST]
					struct create
						: public oridinal, tag_post_method
					{
						param_type operator()(
							  s name
							, s description = ""
							, s trigger = "" ) const
						{
							return boost::assign::map_list_of
								( "name",			name )
								( "description",	description )
								( "trigger",		trigger );
						}
					};

					//event/delete[POST]
					struct delete_
						: public oridinal, tag_post_method
					{
						param_type operator()( s event_id ) const
						{
							return boost::assign::map_list_of( "event_id", event_id );
						}
					};

					//event/add[POST]
					struct add
						: public oridinal, tag_post_method
					{
						param_type operator()( s media_id, s event_id ) const
						{
							return boost::assign::map_list_of
								( "media_id", media_id )
								( "event_id", event_id );
						}
					};

					//event/remove[POST]
					struct remove
						: public oridinal, tag_post_method
					{
						param_type operator()( s event_id, s media_id ) const
						{
							return boost::assign::map_list_of
								( "event_id", event_id )
								( "media_id", media_id );
						}
					};
				}

				//tags
				namespace tags
				{
					//tags/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s tag ) const
						{
							return boost::assign::map_list_of( "tag",	tag );
						}
					};

					//tags/create[POST]
					struct create
						: public oridinal, tag_post_method
					{
						param_type operator()(
							  s media_id
							, s tags ) const
						{
							return boost::assign::map_list_of
								( "media_id",		media_id )
								( "tags",			tags );
						}
					};

					//tags/delete[POST]
					struct delete_
						: public oridinal, tag_post_method
					{
						param_type operator()( s media_id, s tag_id ) const
						{
							return boost::assign::map_list_of
								( "media_id",	media_id )
								( "tag_id",		tag_id );
						}
					};
				}

			} // - api
		} // - twitpic
	} // - clients
} // - webapp

#endif /*YUTOPP_TWITPIC_REQUEST_HPP*/
