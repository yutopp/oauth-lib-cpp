//
// version.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_OAUTH_VERSION_HPP
#define YUTOPP_OAUTH_VERSION_HPP

#include <string>

namespace webapp
{
	namespace oauth
	{
		//version list of oauth
		namespace version
		{
			//
			struct _1_0
			{
				static const std::string& get()
				{
					static const std::string version = "1.0";
					return version;
				}
			};
			struct _1_0a
			{
				static const std::string& get()
				{
					static const std::string version = "1.0";
					return version;
				}
			};
			struct _2_0
			{
				static const std::string& get()
				{
					static const std::string version = "2.0";
					return version;
				}
			};
		}
	} // - oauth
} // - webapp

#endif /*YUTOPP_OAUTH_VERSION_HPP*/
