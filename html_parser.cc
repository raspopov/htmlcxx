#include "html_parser.h"

#include <iostream>
#include <strstream>

//#define DEBUG
#include "debug.h"

#define TAG_NAME_MAX 10
namespace htmlcxx {
	namespace html { 

		using namespace std;
		tree<html::node> parser::parse(const string &html) {

			html_tree.clear();
			tree<html::node>::iterator top = html_tree.begin();
			html::node lambda_node;
			lambda_node.set_offset(0);
			lambda_node.set_length(html.size());
			lambda_node.is_tag(true);
			lambda_node.is_comment(false);
			lambda = html_tree.insert(top,lambda_node);
			this->current_state = lambda;

			const char *c = html.c_str();
			//I really dunno why this assertion fails sometimes
			//assert(html.length() == strlen(html.c_str()));
			const char *end = c + html.length();
			start = c;

			DEBUGP("Parsed text size: %u\n", html.size());

			//Special case for empty strings
			if(html.size() == 0) {
				html::node txt_node;
				txt_node.set_tagname(string());
				txt_node.set_text(string());
				txt_node.set_offset(0);
				txt_node.set_length(0);
				txt_node.is_tag(false);
				txt_node.is_comment(false);
				//Add child content node, but do not update current state
				html_tree.append_child(current_state, txt_node);
				return html_tree;
			}

			while(c < end){

				char const *b = c;
				if(*c++ == '<'){
					if(c == end) break;

					if(isalpha(*c) || *(c + 1) != 0 && *c == '/' && isalpha(*(c+1))){
						//it is a html tag
						DEBUGP("Parsing tag\n");
						parse_html_tag(c, end);
						continue;
					}

					if(++c == end) break;

					if(*c == '-' && c+1 != end && c[1] == '-'){
						DEBUGP("Parsing comment\n");
						//it is a comment. Find where it ends and treat as text
						while ( c != end ) {
							if ( *c++ == '-' && c != end && *c == '-' ) {
								char const *const d = c;
								while ( ++c != end && isspace( *c ) );
								if ( c == end || *c++ == '>' ) break;
								c = d;
							}
						}
						parse_comment(b, c, end);
						continue;
					}

					//Something like <!DOCUMENT
					this->skip_html_tag(c, end);
					parse_comment(b, c, end);
					continue;

				} else {
					//Run of text
					DEBUGP("Reading text...\n");
					for(; c != end; ++c){
						if(*c == '<'){
							//found a new potential html tag. Stop collecting text and
							//store what was found so far
							DEBUGP("Text finished\n");
							break;
						}
					}
					parse_content(b, c, end);

				}
				//print_tree(html_tree, html_tree.begin(), html_tree.end());

			}
			//Correct never closing tags limits
			//FIXME if <BODY> never closes?
			//print_tree(html_tree, html_tree.begin(), html_tree.end());
			return html_tree;
		}

		void parser::parse_comment(char const *b, char const *c, char const *end){
			DEBUGP("Creating comment node %s\n", (string(b, c - b)).c_str());
			html::node com_node;
			//FIXME: set_tagname shouldn't be needed, but first I must check
			//legacy code
			com_node.set_tagname(string(b, c - b));
			com_node.set_text(string(b, c - b));
			com_node.set_offset((uint)(b - start));
			com_node.set_length((uint)(c - b));
			com_node.is_tag(false);
			com_node.is_comment(true);
			//Add child content node, but do not update current state
			html_tree.append_child(current_state, com_node);
		}

		void parser::parse_content(char const *b, char const *c, char const *end){
			DEBUGP("Creating text node %s\n", (string(b, c - b)).c_str());
			html::node txt_node;
			//FIXME: set_tagname shouldn't be needed, but first I must check
			//legacy code
			txt_node.set_tagname(string(b, c - b));
			txt_node.set_text(string(b, c - b));
			txt_node.set_offset((uint)(b - start));
			txt_node.set_length((uint)(c - b));
			txt_node.is_tag(false);
			txt_node.is_comment(false);
			//Add child content node, but do not update current state
			html_tree.append_child(current_state, txt_node);
		}


		void parser::parse_html_tag(char const *&c, char const *end){

			if(c == end) return;

			char const *const tag_begin = c;
			skip_html_tag(c, end);
			char const *const tag_end = c -1;
			bool const is_end_tag = *tag_begin == '/';

			char *name = (char*)malloc(TAG_NAME_MAX + 2); // for \0 and \/

			{
				char *to = name;
				char const *from = tag_begin;

				while ( from != tag_end && !isspace( *from ) ) {
					//If tag is invalid, put a very stupid name for it,
					//so == comparison always fail with a valid tag
					if ( to - name >= TAG_NAME_MAX + is_end_tag ) {
						to = name;
						*to++ = '\1';
						break;
					}
					*to++ = *from++;
				}
				*to = '\0';
			}

			if(!is_end_tag){
				DEBUGP("Found opening tag %s\n" , name);
				html::node tag_node;
				tag_node.set_offset((uint)(tag_begin - start - 1));
				//by now, length is just the size of the tag
				tag_node.set_length(tag_end - tag_begin + 2);
				tag_node.set_tagname(string(name));
				tag_node.set_text(string(tag_begin - 1, tag_end - tag_begin + 2));
				tag_node.is_tag(true);
				tag_node.is_comment(false);

				//append to current tree node
				tree<html::node>::iterator next_state;
				next_state = html_tree.append_child(this->current_state, tag_node);
				this->current_state = next_state;
			} else {
				DEBUGP("Found closing tag %s\n", name);
				//Look if there is a pending open tag with that same name upwards
				//If current_state tag isn't matching tag, maybe a some of its parents
				// matches
				vector< tree<html::node>::iterator > path;
				tree<html::node>::iterator i = current_state;
				bool found_open = false;
				assert(html_tree.size());
				while(i != html_tree.begin()){
#ifdef DEBUG
					cerr << "comparing " << name + 1 << " with " << i->get_tagname()<<endl<<":";
					if(!i->get_tagname().length()) {
						cerr << "Tag with no name at" << i->get_offset()<<";"<<i->get_offset()+i->get_length();
					}
#endif
					assert(i->is_tag());
					assert(i->get_tagname().length());

					bool equal;
					const char *open = i->get_tagname().c_str();
					const char *close = name + 1; //+1 skips \/
					if(strlen(open) != strlen(close)) {
						equal = false;
					} else {
						equal = !(strcasecmp(open,close));
					}


					if(equal) {
						DEBUGP("Found matching tag %s\n", i->get_tagname().c_str());
						//Closing tag closes this tag
						//Set length to full range between the opening tag and
						//closing tag
						i->set_length((tag_end - start) - i->get_offset() + 1);
						i->set_closing_text(string(tag_begin - 1, tag_end - tag_begin + 2));

						this->current_state = html_tree.parent(i);
						found_open = true;
						break;
					} else {
						path.push_back(i);
					}
					i = html_tree.parent(i);
				}
				if(found_open){
					//If match was upper in the tree, so we need to invalidate child
					//nodes that were waiting for a close
					for(uint j = 0; j < path.size(); ++j){
						html_tree.flatten(path[j]);
					}
				} else {
					//If there is no matching opening tag in the tree, treat
					//tag as simple text
					DEBUGP("Unmatched tag %s\n", (string(tag_begin, tag_end - tag_begin)).c_str());
					html::node txt_node;
					txt_node.set_offset((uint)(tag_begin - start));
					//by now, length is just the size of the tag
					txt_node.set_length(tag_end - tag_begin);
					txt_node.set_text(string(tag_begin - 1, tag_end - tag_begin + 2));
					txt_node.is_tag(false);
					txt_node.is_comment(false);

					//This is here only for backward compatibility.
					//Using get_tagname with a text node is deprecated
					txt_node.set_tagname(string(tag_begin, tag_end - tag_begin));
					html_tree.append_child(current_state, txt_node);
				}

			}
			free(name);
		}

		void parser::skip_html_tag(char const *&c, char const *end){


			while(c != end){
				if (*c == '=') { // found an attribute
					++c;
					while (c != end && isspace(*c)) ++c;
					if(*c == '\"' || *c == '\'') {
						char quote = *c++;
						const char *save = c;
						while (c != end && *c != quote) ++c;
						if (c != end) {
							++c;
						} else {
							c = save;
							while (c != end && !isspace(*c) && *c != '>') ++c;
							if (c != end) ++c;
						}
						DEBUGP("Quotes: %s\n", (string(save - 1, c - save  + 1)).c_str());
					}
				}
				if( *c++ == '>') break;
			}

		}

		void node::parse_attributes() {
			if (!is_tag()) return;

			string::size_type a;
			a = text.find("<");
			if (a == string::npos) return;
			++a;

			// Skip initial blankspace
			while (a < text.length() && isspace(text[a])) ++a;

			// Skip tagname
			if (!isalpha(text[a])) return;
			while (a < text.length() && isalpha(text[a])) ++a;

			// Skip blankspace after tagname
			while (a < text.length() && isspace(text[a])) ++a;

			while (a < text.length() && (text[a] != '>')) {
				string key, val;
				while (a < text.length() && !isalpha(text[a]) && !isspace(text[a])) ++a;
				while (a < text.length() && isalpha(text[a])) key += text[a++];
				while (a < text.length() && isspace(text[a])) ++a;
				if (text[a] == '=') {
					++a;
					while (a < text.length() && isspace(text[a])) ++a;
					if (text[a] == '"') {
						string::size_type b = text.find('"', a+1);
						if (b == string::npos) {
							b = text.find_first_of(" >", a+1);
							if (b == string::npos) return;
						}
						val = text.substr(a+1, b-a-1);
					} else if (text[a] == '\'') {
						string::size_type b = text.find('\'', a+1);
						if (b == string::npos) {
							b = text.find_first_of(" >", a+1);
							if (b == string::npos) return;
						}
						val = text.substr(a+1, b-a-1);
					} else {
						while (a < text.length() && !isspace(text[a]) && text[a] != '>') {
							val += text[a++];
						}
					}

					attributes[key] = val;
				}
			}
		}

		bool node::operator==(const node &n) const {
			if((!is_tag()) || (!n.is_tag())) return false;
			return !(strcasecmp(get_tagname().c_str(), n.get_tagname().c_str()));
		}


		ostream &operator<<(ostream &stream, const tree<html::node> &tr) {

			tree<html::node>::pre_order_iterator it = tr.begin();
			tree<html::node>::pre_order_iterator end = tr.end();

			int rootdepth = tr.depth(it);
			stream << "-----" << endl;

			while( it != end ) {

				for(int i=0; i < tr.depth(it) - rootdepth; ++i) stream << "  ";
				stream << "[" << it->get_offset() << ";";
				stream << it->get_offset() + it->get_length() << ")";
				stream << (string)(*it) << endl;
				++it;
			}
			stream << "-----" << endl;
			return stream;
		}
		node::operator string() const {
			if(is_tag()) {
				return this->get_tagname();
			} 
			return this->get_text();
		}
		ostream &node::operator<<(ostream &stream) const {
			stream << (string)(*this);
			return stream;
		}
			

	}//namespace html
}//namespace htmlcxx
