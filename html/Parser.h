#ifndef __HTML_PARSER_H__
#define __HTML_PARSER_H__

#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

#include "Node.h"
#include "tree.h"

namespace htmlcxx {
	namespace HTML {
		class Parser {

			public:
				Parser() : mpStart(0), mpLiteral(0), mCdata(false) {}
				~Parser() {}
				/** Parse the html code
				  * Access to the tree and subsequent calls to this
				  * method are thread _unsafe_.
				  */
				const tree<HTML::Node> &parse(const std::string &html);
			protected:

				void parseHtmlTag(char const *&c, char const *end);
				const char *skipHtmlTag(char const *ptr, char const *end);
				const char *skipHtmlComment(char const *ptr, char const *end);
				void parseContent(char const *b, char const *c, char const *end);
				void parseComment(char const *b, char const *c, char const *end);

				tree<HTML::Node> mHtmlTree;
				const char *mpStart;
				const char *mpLiteral;
				bool mCdata;
				tree<HTML::Node>::iterator mCurrentState;
		};
		std::ostream &operator<<(std::ostream &stream, const tree<HTML::Node> &tr);
	}//namespace HTML
}//namespace htmlcxx

#endif
