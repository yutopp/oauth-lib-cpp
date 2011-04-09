#ifndef YUTOPP_TWITTER_STREAMING_REQUEST_HPP
#define YUTOPP_TWITTER_STREAMING_REQUEST_HPP

#include "../../basic_api.hpp"

namespace webapp
{
	namespace clients
	{
		namespace twitter
		{
			namespace api
			{
				using namespace ::webapp::clients::api;

				//Streaming API
				namespace streaming
				{
					//
					struct firehose
						: public tag_get_method
					{
						param_type operator()(
							  s count
							, s delimited ) const
						{
							return boost::assign::map_list_of
									( "count",		count )
									( "delimited",	delimited );
						}
					};

					//
					struct sample
						: public tag_get_method
					{
						param_type operator()( s delimited ) const
						{
							return boost::assign::map_list_of
									( "delimited",	delimited );
						}
					};

					//
					struct filter
						: public tag_post_method
					{
						param_type operator()(
							  s follow
							, s track
							, s locations
							, s count
							, s delimited ) const
						{
							return boost::assign::map_list_of
									( "follow",		follow )
									( "track",		track )
									( "locations",	locations )
									( "count",		count )
									( "delimited", delimited );
						}
					};

					//
					struct links
						: public tag_get_method
					{
						param_type operator()( s delimited ) const
						{
							return boost::assign::map_list_of
									( "delimited", delimited );
						}
					};

					//
					struct retweet
						: public tag_get_method
					{
						param_type operator()( s delimited ) const
						{
							return boost::assign::map_list_of
									( "delimited", delimited );
						}
					};
				}
			} // - api
		} // - twitter
	} // - clients
} // - webapp

#endif /*YUTOPP_TWITTER_STREAMING_REQUEST_HPP*/
