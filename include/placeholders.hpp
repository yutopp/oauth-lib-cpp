//
// placeholders.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_PLACEHOLDERS_HPP
#define YUTOPP_PLACEHOLDERS_HPP

#include <boost/bind/arg.hpp>

namespace webapp
{
	namespace placeholders
	{
		namespace {
			static boost::arg<1> value;		// 返り値
			static boost::arg<1> response;	// responseに整形された返り値
			static boost::arg<2> error;		// エラーコード
		}
	} // - placeholders
} // - webapp

#endif /*YUTOPP_PLACEHOLDERS_HPP*/