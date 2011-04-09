#ifndef YUTOPP_TWITTER_REST_REQUEST_HPP
#define YUTOPP_TWITTER_REST_REQUEST_HPP

#include "../../basic_api.hpp"

#include <boost/filesystem/path.hpp>			//ファイルパスの解析
#include <boost/algorithm/string/case_conv.hpp>	//tolower

namespace webapp
{
	namespace clients
	{
		namespace twitter
		{
			namespace api
			{
				using namespace ::webapp::clients::api;

				//
				namespace statuses
				{
					//statuses/home_timeline[GET]
					struct home_timeline
						: public encoded_data, tag_get_method
					{
						param_type operator()(
							  s since_id = ""
							, s trim_user = ""
							, s include_entities = ""
							) const
						{
							return boost::assign::map_list_of
									( "since_id",			since_id )
									( "trim_user",			trim_user )
									( "include_entities",	include_entities );
						}
					};

					//statuses/show[GET]
					struct show
						: public encoded_data, tag_get_method
					{
						param_type operator()( s id ) const
						{
							return boost::assign::map_list_of
									( "id", id );
						}
					};

					//statuses/update[POST]
					struct update
						: public encoded_data, tag_post_method
					{
						param_type operator()(
							  s status
							, s in_reply_to_status_id = ""
							, s g_lat = ""
							, s g_long = ""
							, s place_id = ""
							, s display_coordinates = ""
							, s source = "" ) const
						{
							return boost::assign::map_list_of
									( "status",					status )
									( "in_reply_to_status_id",	in_reply_to_status_id )
									( "lat",					g_lat )
									( "long",					g_long )
									( "place_id",				place_id )
									( "display_coordinates",	display_coordinates )
									( "source",					source );
						}
					};

					//statuses/friends[POST]
					struct friends
						: public encoded_data, tag_post_method
					{
						param_type operator()(
							  s id = ""
							, s user_id = ""
							, s screen_name = ""
							, s page = ""
							, s lite = ""
							, s since = ""
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
									( "id",					id )
									( "user_id",			user_id )
									( "screen_name",		screen_name )
									( "page",				page )
									( "lite",				lite )
									( "since",				since )
									( "cursor",				cursor );
						}
					};
				}

				//アカウント関連のAPI(多分全部)
				namespace account
				{
					namespace detail_
					{
						struct post_image
							: public form_data, tag_post_method
						{
							param_type operator()( s image ) const
							{
								return boost::assign::map_list_of
										( "image", image );
							}

							//
							static string_type generate_body( s boundary, const param_type& param )
							{
								//パス
								const boost::filesystem::path path = param.at( "image" );
								const auto ext( boost::algorithm::to_lower_copy( path.extension().string() ) );
								if ( ( ext != ".png" ) && ( ext != ".gif" ) && ( ext != ".jpg" ) && ( ext != ".jpeg" ) )
									throw std::exception( "bad file." );
							
								//バイナリ読み込み
								const auto bin( load_binary_file( path.string() ) );
							
								//
								std::stringstream ss;
								ss << "--" << boundary << "\r\n";
								ss << "Content-Disposition: form-data; name=\"image\"; filename=\"" << util::encoding::u8( path.filename().string() ) << "\"" << "\r\n";
								ss << "Content-Type: image/" << std::string( ++ext.begin(), ext.end() ) << "\r\n";
								ss << "Content-Transfer-Encoding: binary" << "\r\n";
								ss << "Content-Length: " << bin.size() << "\r\n";
								ss << "\r\n";
								ss.write( (char*)&bin[0], bin.size() );
								ss << "\r\n";
								ss << "--" << boundary << "--" << std::flush;

								return ss.str();
							}
						};
					}

					//account/update_delivery_device[POST]
					struct update_delivery_device
						: public encoded_data, tag_post_method
					{
						param_type operator()( s device ) const
						{
							return boost::assign::map_list_of
								( "device", device );
						}
					};

					//account/update_profile_colors[POST]
					struct update_profile_colors
						: public encoded_data, tag_post_method
					{
						param_type operator()(
							  s profile_background_color = ""
							, s profile_text_color = ""
							, s profile_link_color = ""
							, s profile_sidebar_fill_color = ""
							, s profile_sidebar_border_color = "" ) const
						{
							return boost::assign::map_list_of
								( "profile_background_color",		profile_background_color )
								( "profile_text_color",				profile_text_color )
								( "profile_link_color",				profile_link_color )
								( "profile_sidebar_fill_color",		profile_sidebar_fill_color )
								( "profile_sidebar_border_color",	profile_sidebar_border_color );
						}
					};

					//account/update_profile_image[POST]
					struct update_profile_image
						: public detail_::post_image
					{};

					//account/update_profile_background_image[POST]
					struct update_profile_background_image
						: public detail_::post_image
					{};

					//account/rate_limit_status[GET]
					struct rate_limit_status
						: public encoded_data, tag_get_method
					{
						param_type operator()() const
						{
							return param_type();
						}
					};

					//account/update_profile[POST]
					struct update_profile
						: public encoded_data, tag_post_method
					{
						param_type operator()(
							  s name = ""
							, s url = ""
							, s location = ""
							, s description = "" ) const
						{
							return boost::assign::map_list_of
								( "name", name )
								( "url", url )
								( "location", location )
								( "description", description );
						}
					};
				}

				// list系API
				//  - なんでこんなことになったんだろう。泣きたい。
				namespace list
				{
					namespace detail_
					{
						struct oridinal_arg1
							: public oridinal
						{
							//
							static param_type generate_header_param( param_type param )
							{
								param.erase( "__username" );
								return encode_parameter( param );
							}

							static string_type generate_body( param_type param )
							{
								param.erase( "__username" );
								return generate_body_helper( encode_parameter( param ) );
							}

							static boost::format generate_url( boost::format&& formatter, const param_type& param )
							{
								return formatter % param.at( "__username" );
							}
						};

						struct oridinal_arg2
							: public oridinal
						{
							//
							static param_type generate_header_param( param_type param )
							{
								param.erase( "__username" );
								param.erase( "__list_id" );
								return encode_parameter( param );
							}

							static string_type generate_body( param_type param )
							{
								param.erase( "__username" );
								param.erase( "__list_id" );
								return generate_body_helper( encode_parameter( param ) );
							}

							static boost::format generate_url( boost::format&& formatter, const param_type& param )
							{
								return formatter % param.at( "__username" ) % param.at( "__list_id" );
							}
						};

						struct oridinal_arg2_all
							: public oridinal
						{
							//
							static param_type generate_header_param( param_type param )
							{
								return param_type();
							}

							static string_type generate_body( param_type param )
							{
								return "";
							}

							static boost::format generate_url( boost::format&& formatter, const param_type& param )
							{
								return formatter % param.at( "__username" ) % param.at( "__list_id" );
							}
						};

						struct oridinal_arg3_all
							: public oridinal
						{
							//
							static param_type generate_header_param( param_type param )
							{
								return param_type();
							}

							static string_type generate_body( param_type param )
							{
								return "";
							}

							static boost::format generate_url( boost::format&& formatter, const param_type& param )
							{
								return formatter % param.at( "__username" ) % param.at( "__list_id" ) % param.at( "__id" );
							}
						};
					}

					// - list 関連のAPI - 

					//<username>/lists[POST]
					struct create
						: public detail_::oridinal_arg1, tag_post_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s name
							, s mode = ""
							, s description = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "name",			name )
								( "mode",			mode )
								( "description",	description );
						}
					};

					//<username>/lists/<id>[POST]
					struct update
						: public oridinal, tag_post_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s id				//この引数は送信されないよ！
							, s name
							, s mode
							, s description = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__id",			id )
								( "name",			name )
								( "mode",			mode )
								( "description",	description );
						}
						//
						static param_type generate_header_param( param_type param )
						{
							param.erase( "__username" );
							param.erase( "__id" );
							return encode_parameter( param );
						}

						static string_type generate_body( param_type param )
						{
							param.erase( "__username" );
							param.erase( "__id" );
							return generate_body_helper( encode_parameter( param ) );
						}

						static boost::format generate_url( boost::format&& formatter, const param_type& param )
						{
							return formatter % param.at( "__username" ) % param.at( "__id" );
						}
					};

					//<username>/lists[GET]
					struct index
						: public detail_::oridinal_arg1, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "cursor",			cursor );
						}
					};

					//<username>/lists/<id>[GET]
					struct show
						: public oridinal, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s id				//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__id",			id );
						}
						//
						static param_type generate_header_param( param_type param )
						{
							return param_type();
						}

						static string_type generate_body( param_type param )
						{
							return "";
						}

						static boost::format generate_url( boost::format&& formatter, const param_type& param )
						{
							return formatter % param.at( "__username" ) % param.at( "__id" );
						}
					};

					//<username>/lists/<id>[DELETE]
					struct destroy
						: public oridinal, tag_delete_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s id				//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__id",			id );
						}
						//
						static param_type generate_header_param( param_type param )
						{
							return param_type();
						}

						static string_type generate_body( param_type param )
						{
							return "";
						}

						static boost::format generate_url( boost::format&& formatter, const param_type& param )
						{
							return formatter % param.at( "__username" ) % param.at( "__id" );
						}
					};

					//<username>/lists/<list_id>/statuses[GET]
					struct statuses
						: public detail_::oridinal_arg2, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s since_id = ""
							, s max_id = ""
							, s per_page = ""
							, s page = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "since_id",		since_id )
								( "max_id",			max_id )
								( "per_page",		per_page )
								( "page",			page );
						}
					};

					//<username>/lists/memberships[GET]
					struct memberships
						: public detail_::oridinal_arg1, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "cursor",			cursor );
						}
					};

					//<username>/lists/subscriptions[GET]
					struct subscriptions
						: public detail_::oridinal_arg1, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "cursor",			cursor );
						}
					};


					// - 登録内容に関する API - 

					//<username>/<list_id>/members[GET]
					struct get_member
						: public detail_::oridinal_arg2, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "cursor",			cursor );
						}
					};

					//<username>/<list_id>/members[POST]
					struct add_member
						: public detail_::oridinal_arg2, tag_post_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s id ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "id",				id );
						}
					};

					//<username>/<list_id>/members[DELETE]
					struct delete_member
						: public detail_::oridinal_arg2, tag_delete_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s id ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "id",				id );
						}
					};

					//<username>/<list_id>/members/<id>[GET]
					struct is_member
						: public detail_::oridinal_arg3_all, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s id				//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "__id",			id );
						}
					};


					// - list の購読に関する API - 

					//<username>/<list_id>/subscribers[GET]
					struct get_subscribers
						: public detail_::oridinal_arg2, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s cursor = "" ) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "cursor",			cursor );
						}
					};

					//<username>/<list_id>/subscribers[POST]
					struct subscribe
						: public detail_::oridinal_arg2_all, tag_post_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id );
						}
					};

					//<username>/<list_id>/subscribers[DELETE]
					struct unsubscribe
						: public detail_::oridinal_arg2_all, tag_delete_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id );
						}
					};

					//<username>/<list_id>/subscribers/<id>[GET]
					struct is_subscriber
						: public detail_::oridinal_arg3_all, tag_get_method
					{
						param_type operator()(
							  s username		//この引数は送信されないよ！
							, s list_id			//この引数は送信されないよ！
							, s id				//この引数は送信されないよ！
							) const
						{
							return boost::assign::map_list_of
								( "__username",		username )
								( "__list_id",		list_id )
								( "__id",			id );
						}
					};
				}

				//
				namespace geo
				{
					struct reverse_geocode
						: public encoded_data, tag_get_method
					{
						param_type operator()(
							  s g_lat
							, s g_long
							, s accuracy = ""
							, s granularity = ""
							, s max_results = "" ) const
						{
							return boost::assign::map_list_of
									( "lat",					g_lat )
									( "long",					g_long )
									( "accuracy",				accuracy )
									( "granularity",			granularity )
									( "max_results",			max_results );
						}
					};
				}
			} // - api
		} // - twitter
	} // - clients
} // - webapp

#endif /*YUTOPP_TWITTER_REST_REQUEST_HPP*/
