#ifndef YUTOPP_BASIC_API_HPP
#define YUTOPP_BASIC_API_HPP

#include <string>
#include <map>

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace webapp
{
	namespace clients
	{
		namespace api
		{
			typedef std::string			string_type;
			typedef const string_type&	s;	//いちいち書くのがめんどくさかったんですごめんなさい許してください！

			typedef std::map<string_type, string_type> param_type;

			//
			struct encoded_data {};	//application/x-www-form-urlencoded
			struct atom_data {};	//application/atom+xml
			struct form_data {};	//multipart/form-data
			struct oridinal {};		//

			//
			struct tag_get_method {};
			struct tag_post_method {};
			struct tag_put_method {};
			struct tag_delete_method {};

			//
			template<class T, class = void>
			struct method_name;

			template<class T>
			struct method_name<T, typename boost::enable_if<boost::is_base_of<tag_get_method, T> >::type>
			{
				static const string_type& value()
				{
					const static string_type method = "GET";
					return method;
				}
			};

			template<class T>
			struct method_name<T, typename boost::enable_if<boost::is_base_of<tag_post_method, T> >::type>
			{
				static const string_type& value()
				{
					const static string_type method = "POST";
					return method;
				}
			};

			template<class T>
			struct method_name<T, typename boost::enable_if<boost::is_base_of<tag_put_method, T> >::type>
			{
				static const string_type& value()
				{
					const static string_type method = "PUT";
					return method;
				}
			};

			template<class T>
			struct method_name<T, typename boost::enable_if<boost::is_base_of<tag_delete_method, T> >::type>
			{
				static const string_type& value()
				{
					const static string_type method = "DELETE";
					return method;
				}
			};
		} // - api
	} // - clients
} // - webapp

#endif /*YUTOPP_BASIC_API_HPP*/
