#ifndef YUTOPP_HATENA_REQUEST_HPP
#define YUTOPP_HATENA_REQUEST_HPP

#include "../basic_api.hpp"

#include <boost/filesystem/path.hpp>			//ファイルパスの解析
#include <boost/algorithm/string/case_conv.hpp>	//tolower

namespace webapp
{
	namespace clients
	{
		namespace hatena
		{
			namespace api
			{
				using namespace ::webapp::clients::api;

				//
				namespace bookmark
				{
					//statuses/home_timeline[GET]
					struct endpoint
						: public atom_data, tag_get_method
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
				}


			} // - api
		} // - hatena
	} // - clients
} // - webapp

#endif /*YUTOPP_HATENA_REQUEST_HPP*/
