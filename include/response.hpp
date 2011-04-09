//
// response.hpp
// ~~~~~~~~
//

#ifndef YUTOPP_RESPONSE_HPP
#define YUTOPP_RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace webapp
{
	namespace response_
	{
		//ちょこっとだけ便利なはず！
		class response
		{
			typedef response												self_type;
			typedef boost::shared_ptr<self_type>							self_smart_pointer;

		public:
			typedef std::string												index_type;
			typedef std::string												data_type;
			typedef boost::unordered_map<std::string, self_smart_pointer>	index_list_type;
			typedef std::vector<self_smart_pointer>							node_type;


			typedef node_type::iterator										iterator;		// (ﾟДﾟ)
			typedef node_type::const_iterator								const_iterator;	// (ﾟДﾟ)

			response()
				: name_( "__parent" )
			{}

			explicit response( const index_type& index )
				: name_( index )
			{}

			response( const index_type& index, const data_type& data )
				: name_( index )
				, data_( data )
			{}

			self_smart_pointer makeNode( const index_type& index )
			{
				auto p = boost::make_shared<self_type>( index );
				child_.push_back( p );
				index_.insert( index_list_type::value_type( index, p ) );
				return p;
			}

			void setValue( const data_type& data )
			{
				data_ = data;
			}
		
			const index_type& getName() const
			{
				return name_;
			}

			const data_type getData() const
			{
				return *data_;
			}

			const response& operator()( const index_type& index ) const
			{
				return *index_.at( index );
			}

			operator data_type() const
			{
				return getData();
			}

			bool hasData() const
			{
				return data_;
			}

			//iterator begin() { return child_.begin(); }
			//iterator end() { return child_.end(); }

			const_iterator begin() const { return child_.cbegin(); }
			const_iterator end() const { return child_.cend(); }

		private:
			index_list_type index_;
			node_type child_;

			index_type name_;
			boost::optional<data_type> data_;
		};

		inline std::ostream& operator<<( std::ostream& ss, const response& res )
		{
			ss << res.getData();
			return ss;
		}

		namespace tree_builder
		{
			template<class Tree>
			void build_xml_tree( const Tree& tree, response& res )
			{
				BOOST_FOREACH( const auto& val, tree ) {
					auto child = res.makeNode( util::encoding::sjis( val.first ) );
					const auto& node = val.second;

					if ( node.size() == 0 ) {
						child->setValue( util::encoding::sjis( node.data() ) );
					} else {
						build_xml_tree( node, *child );
					}
				}
			}

			//depricated!!!
			template<class Tree>
			void build_json_tree( const Tree& tree, response& res )
			{
				BOOST_FOREACH( const auto& val, tree ) {
					auto child = res.makeNode( val.first );
					const auto& node = val.second;

					if ( node.size() == 0 ) {
						child->setValue( node.data() );
					} else {
						build_json_tree( node, *child );
					}
				}
			}

		} // - tree_builder
	} // - response_
	using namespace response_;	//(ﾟДﾟ)



	//xmlを解析してresponseを返すヘルパ
	template<class T>
	response xml_tree_helper( const T& src )
	{
		try {
			std::stringstream ss;
			ss << util::encoding::sjis( src );

			boost::property_tree::basic_ptree<T, T> root;
			boost::property_tree::read_xml( ss, root );

			response res;
			tree_builder::build_xml_tree( root, res );
			return res;
		}
		catch( const boost::property_tree::xml_parser_error& e )
		{
			return response( "__xml_tree_helper", e.what() );
		}
	}

	//jsonを解析してresponseを返すヘルパ
	//depricated!!!
	template<class T>
	response json_tree_helper( const T& src )
	{
		using namespace ::oauth;

		std::stringstream ss;
		ss << []( const T& src ) -> std::string {
			std::string out;

			for( auto it = src.begin(); it<src.end(); ) {
				if ( *it == '\\' ) {
					if ( *(it+1) == 'u' ) {
						it+=2;
						std::string uni( &(*it), 4 );
						long stri = strtol( uni.c_str(), NULL, 16 );

						it += 4;
						out += babel::unicode_to_sjis( std::wstring( 1, stri ) );

						continue;
					}
				}
				out += (*it++);
			}
			return out;
		}( util::sjis( src ) );

		boost::property_tree::ptree root;
		boost::property_tree::read_json( ss, root );

		response res;
		tree_builder::build_json_tree( root, res );
		return res;
	}

} // - webapp

#endif /*YUTOPP_RESPONSE_HPP*/