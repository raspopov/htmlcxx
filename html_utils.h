#ifndef __HTML_UTILS_H__
#define __HTML_UTILS_H__

#include "html_parser.h"

namespace htmlcxx {
	namespace html {

		/** Trim the argument string at both end and convert sibling blanks into a 
		  * single space */
		std::string single_blank(const std::string& str);
		/** Remove html comments from the string */
		std::string strip_comments(const std::string& str);
		/** Convert html entities into their ISO8859-1 equivalents */
		std::string decode_entities(const std::string& str);
		/** Get the value of an attribute inside the text of a tag 
		  * @return empty string if the attribute is not found, or the value of 
 		  * the attribute
		  */
		std::string get_attribute(const std::string& tag, const std::string& attr);
		/** Create absolute url for a link */
		std::string convert_link(const std::string& relative, const std::string& root);
		/** Create a gml representation of the tree for input of tools like graphviz */
		std::string serialize_gml(const tree<node> &tr); 
	}
}

#endif
