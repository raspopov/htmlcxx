#ifndef __HTML_PARSER_H__
#define __HTML_PARSER_H__

#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "tree.h"

namespace htmlcxx {
	namespace HTML {

		class Node {

			public:
				Node() {}
				~Node() {}

				inline void text(const std::string& text) { this->mText = text; }
				inline const std::string& text() const { return this->mText; }

				inline void closingText(const std::string &text) { this->mClosingText = text; }
				inline const std::string& closingText() const { return mClosingText; }

				inline void offset(uint offset) { this->mOffset = offset; }
				inline uint offset() const { return this->mOffset; }

				inline void length(uint length) { this->mLength = length; }
				inline uint length() const { return this->mLength; }

				inline void tagName(const std::string& tagname) { this->mTagName = tagname; }
				inline const std::string& tagName() const { return this->mTagName; }

				bool isTag() const { return this->mIsHtmlTag; }
				void isTag(bool is_html_tag){ this->mIsHtmlTag = is_html_tag; }

				bool isComment() const { return this->mComment; }
				void isComment(bool comment){ this->mComment = comment; }

				std::string attribute(const std::string &attr) const { 
					std::map<std::string, std::string>::const_iterator i = this->mAttributes.find(attr);
					if (i != this->mAttributes.end()) {
						return i->second;
					} else {
						return std::string();
					}
				}

				operator std::string() const;
				std::ostream &Node::operator<<(std::ostream &stream) const;

				std::map<std::string, std::string> getAttributes() const { return this->mAttributes; }
				void parseAttributes();

				bool operator==(const Node &rhs) const;

			protected:

				std::string mText;
				std::string mClosingText;
				uint mOffset;
				uint mLength;
				std::string mTagName;
				std::map<std::string, std::string> mAttributes;
				bool mIsHtmlTag;
				bool mComment;
		};

		class Parser {

			public:
				Parser(){}
				~Parser(){}
				tree<HTML::Node> parse(const std::string &html);
			protected:

				void parseHtmlTag(char const *&c, char const *end);
				void skipHtmlTag(char const *&c, char const *end);
				void parseContent(char const *b, char const *c, char const *end);
				void parseComment(char const *b, char const *c, char const *end);

				tree<HTML::Node> mHtmlTree;
				const char *mpStart;
				tree<HTML::Node>::iterator mCurrentState;
		};
		std::ostream &operator<<(std::ostream &stream, const tree<HTML::Node> &tr);
	}//namespace HTML
}//namespace htmlcxx

#endif
