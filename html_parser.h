#ifndef __HTML_PARSER_H__
#define __HTML_PARSER_H__

#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "tree.h"

using namespace std;

namespace htmlcxx {
	namespace html {

		class node {

			public:
				node() {}
				~node() {}

				inline void set_text(const string& text) { this->text = text; }
				inline const string& get_text() const { return this->text; }

				inline void set_closing_text(const string &text) { this->closing_text = text; }
				inline const string& get_closing_text() const { return closing_text; }

				inline void set_offset(uint offset) { this->offset = offset; }
				inline uint get_offset() const { return this->offset; }

				inline void set_length(uint length) { this->length = length; }
				inline uint get_length() const { return this->length; }

				inline void set_tagname(string tagname) { this->tagname = tagname; }
				inline const string& get_tagname() const { return this->tagname; }

				bool is_tag() const { return this->is_html_tag; }
				void is_tag(bool is_html_tag){ this->is_html_tag = is_html_tag; }

				bool is_comment() const { return this->comment; }
				void is_comment(bool comment){ this->comment = comment; }

				string get_attribute(const string &attr) const { 
					map<string, string>::const_iterator i = this->attributes.find(attr);
					if (i != this->attributes.end()) {
						return i->second;
					} else {
						return string();
					}
				}

				operator std::string() const;
				ostream &node::operator<<(ostream &stream) const;

				map<string, string> get_attributes() const { return this->attributes; }
				void parse_attributes();

				bool operator==(const node &rhs) const;
			protected:

				string text;
				string closing_text;
				uint offset;
				uint length;
				string tagname;
				map<string, string> attributes;
				bool is_html_tag;
				bool comment;
		};

		class parser {

			public:
				parser(){}
				~parser(){}
				tree<html::node> parse(const std::string &html);
			protected:
				void parse_html_tag(char const *&c, char const *end);
				void skip_html_tag(char const *&c, char const *end);
				void parse_content(char const *b, char const *c, char const *end);
				void parse_comment(char const *b, char const *c, char const *end);

				tree<html::node> html_tree;
				const char *start;
				tree<html::node>::iterator current_state;
				tree<html::node>::iterator lambda;
		};
		ostream &operator<<(ostream &stream, const tree<html::node> &tr);
	}//namespace html
}//namespace htmlcxx

#endif
