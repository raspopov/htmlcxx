#include "parser.h"

#include <iostream>
#include <strstream>

//#define DEBUG
#include "debug.h"

#define TAG_NAME_MAX 10

using namespace std;
namespace htmlcxx 
{
	namespace HTML 
	{ 

		tree<HTML::Node> Parser::parse(const string &html) 
		{

			mHtmlTree.clear();
			tree<HTML::Node>::iterator top = mHtmlTree.begin();
			HTML::Node lambda_node;
			lambda_node.offset(0);
			lambda_node.length(html.size());
			lambda_node.isTag(true);
			lambda_node.isComment(false);
			this->mCurrentState = mHtmlTree.insert(top,lambda_node);

			const char *c = html.c_str();
			//I really dunno why this assertion fails sometimes
			//assert(html.length() == strlen(html.c_str()));
			const char *end = c + html.length();
			mpStart = c;

			DEBUGP("Parsed text size: %u\n", html.size());

			//Special case for empty strings
			if (html.size() == 0) 
			{
				HTML::Node txt_node;
				txt_node.tagName(string());
				txt_node.text(string());
				txt_node.offset(0);
				txt_node.length(0);
				txt_node.isTag(false);
				txt_node.isComment(false);
				//Add child content node, but do not update current state
				mHtmlTree.append_child(mCurrentState, txt_node);
				return mHtmlTree;
			}

			while (c < end)
			{

				char const *b = c;
				if (*c++ == '<')
				{
					if (c == end) break;

					if (isalpha(*c) || *(c + 1) != 0 && *c == '/' && isalpha(*(c+1)))
					{
						//it is a html tag
						DEBUGP("Parsing tag\n");
						parseHtmlTag(c, end);
						continue;
					}

					if (++c == end) break;

					if (*c == '-' && c + 1 != end && c[1] == '-')
					{
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
						parseComment(b, c, end);
						continue;
					}

					//Something like <!DOCUMENT
					this->skipHtmlTag(c, end);
					this->parseComment(b, c, end);
					continue;

				} 
				else 
				{
					//Run of text
					DEBUGP("Reading text...\n");
					for (; c != end; ++c)
					{
						if (*c == '<')
						{
							//found a new potential html tag. Stop collecting text and
							//store what was found so far
							DEBUGP("Text finished\n");
							break;
						}
					}
					this->parseContent(b, c, end);

				}
				//print_tree(mHtmlTree, mHtmlTree.begin(), mHtmlTree.end());

			}
			//Correct never closing tags limits
			//FIXME if <BODY> never closes?
			//print_tree(mHtmlTree, mHtmlTree.begin(), mHtmlTree.end());
			return mHtmlTree;
		}

		void Parser::parseComment(char const *b, char const *c, char const *end)
		{
			DEBUGP("Creating comment node %s\n", (string(b, c - b)).c_str());
			HTML::Node com_node;
			//FIXME: set_tagname shouldn't be needed, but first I must check
			//legacy code
			com_node.tagName(string(b, c - b));
			com_node.text(string(b, c - b));
			com_node.offset((uint)(b - mpStart));
			com_node.length((uint)(c - b));
			com_node.isTag(false);
			com_node.isComment(true);
			//Add child content node, but do not update current state
			mHtmlTree.append_child(mCurrentState, com_node);
		}

		void Parser::parseContent(char const *b, char const *c, char const *end)
		{
			DEBUGP("Creating text node %s\n", (string(b, c - b)).c_str());
			HTML::Node txt_node;
			//FIXME: set_tagname shouldn't be needed, but first I must check
			//legacy code
			txt_node.tagName(string(b, c - b));
			txt_node.text(string(b, c - b));
			txt_node.offset((uint)(b - mpStart));
			txt_node.length((uint)(c - b));
			txt_node.isTag(false);
			txt_node.isComment(false);
			//Add child content node, but do not update current state
			mHtmlTree.append_child(mCurrentState, txt_node);
		}


		void Parser::parseHtmlTag(char const *&c, char const *end)
		{

			if (c == end) return;

			char const * const tag_begin = c;
			this->skipHtmlTag(c, end);

			char const * const tag_end = c -1;
			bool const is_end_tag = *tag_begin == '/';

			char *name = (char*)malloc(TAG_NAME_MAX + 2); // for \0 and \/

			{
				char *to = name;
				char const *from = tag_begin;

				while ( from != tag_end && !isspace( *from ) ) 
				{
					//If tag is invalid, put a very stupid name for it,
					//so == comparison always fail with a valid tag
					if ( to - name >= TAG_NAME_MAX + is_end_tag ) 
					{
						to = name;
						*to++ = '\1';
						break;
					}
					*to++ = *from++;
				}
				*to = '\0';
			}

			if (!is_end_tag) 
			{

				DEBUGP("Found opening tag %s\n" , name);
				HTML::Node tag_node;
				tag_node.offset((uint)(tag_begin - mpStart - 1));
				//by now, length is just the size of the tag
				tag_node.length(tag_end - tag_begin + 2);
				tag_node.tagName(string(name));
				tag_node.text(string(tag_begin - 1, tag_end - tag_begin + 2));
				tag_node.isTag(true);
				tag_node.isComment(false);

				//append to current tree node
				tree<HTML::Node>::iterator next_state;
				next_state = mHtmlTree.append_child(this->mCurrentState, tag_node);
				this->mCurrentState = next_state;

			} 
			else 
			{
				DEBUGP("Found closing tag %s\n", name);
				//Look if there is a pending open tag with that same name upwards
				//If mCurrentState tag isn't matching tag, maybe a some of its parents
				// matches
				vector< tree<HTML::Node>::iterator > path;
				tree<HTML::Node>::iterator i = mCurrentState;
				bool found_open = false;
				while (i != mHtmlTree.begin())
				{
					#ifdef DEBUG
					cerr << "comparing " << name + 1 << " with " << i->get_tagname()<<endl<<":";
					if (!i->get_tagname().length()) cerr << "Tag with no name at" << i->get_offset()<<";"<<i->get_offset()+i->get_length();
					#endif
					assert(i->isTag());
					assert(i->tagName().length());

					bool equal;
					const char *open = i->tagName().c_str();
					const char *close = name + 1; //+1 skips \/
					if (strlen(open) != strlen(close)) equal = false;
					else equal = !(strcasecmp(open,close));


					if (equal) 
					{
						DEBUGP("Found matching tag %s\n", i->tagName().c_str());
						//Closing tag closes this tag
						//Set length to full range between the opening tag and
						//closing tag
						i->length((tag_end - mpStart) - i->offset() + 1);
						i->closingText(string(tag_begin - 1, tag_end - tag_begin + 2));

						this->mCurrentState = mHtmlTree.parent(i);
						found_open = true;
						break;
					} 
					else 
					{
						path.push_back(i);
					}

					i = mHtmlTree.parent(i);
				}
				if (found_open)
				{
					//If match was upper in the tree, so we need to invalidate child
					//nodes that were waiting for a close
					for (uint j = 0; j < path.size(); ++j) mHtmlTree.flatten(path[j]);
				} 
				else 
				{
					//If there is no matching opening tag in the tree, treat
					//tag as simple text
					DEBUGP("Unmatched tag %s\n", (string(tag_begin, tag_end - tag_begin)).c_str());
					HTML::Node txt_node;
					txt_node.offset((uint)(tag_begin - mpStart));
					//by now, length is just the size of the tag
					txt_node.length(tag_end - tag_begin);
					txt_node.text(string(tag_begin - 1, tag_end - tag_begin + 2));
					txt_node.isTag(false);
					txt_node.isComment(false);

					//This is here only for backward compatibility.
					//Using get_tagname with a text node is deprecated
					txt_node.tagName(string(tag_begin, tag_end - tag_begin));
					mHtmlTree.append_child(mCurrentState, txt_node);
				}

			}
			free(name);
		}

		void Parser::skipHtmlTag(char const *&c, char const *end){


			while (c != end)
			{
				if (*c == '=') 
				{ // found an attribute
					++c;
					while (c != end && isspace(*c)) ++c;
					if(*c == '\"' || *c == '\'') 
					{
						char quote = *c++;
						const char *save = c;
						while (c != end && *c != quote) ++c;
						if (c != end) 
						{
							++c;
						} 
						else 
						{
							c = save;
							while (c != end && !isspace(*c) && *c != '>') ++c;
							if (c != end) ++c;
						}
						DEBUGP("Quotes: %s\n", (string(save - 1, c - save  + 1)).c_str());
					}
				}
				if (*c++ == '>') break;
			}

		}

		void Node::parseAttributes() 
		{
			if (!(this->isTag())) return;

			string::size_type a;
			a = mText.find("<");
			if (a == string::npos) return;
			++a;

			// Skip initial blankspace
			while (a < mText.length() && isspace(mText[a])) ++a;

			// Skip tagname
			if (!isalpha(mText[a])) return;
			while (a < mText.length() && isalpha(mText[a])) ++a;

			// Skip blankspace after tagname
			while (a < mText.length() && isspace(mText[a])) ++a;

			while (a < mText.length() && (mText[a] != '>')) 
			{
				string key, val;
				while (a < mText.length() && !isalpha(mText[a]) && !isspace(mText[a])) ++a;
				while (a < mText.length() && isalpha(mText[a])) key += mText[a++];
				while (a < mText.length() && isspace(mText[a])) ++a;
				if (mText[a] == '=') 
				{
					++a;
					while (a < mText.length() && isspace(mText[a])) ++a;
					if (mText[a] == '"') 
					{
						string::size_type b = mText.find('"', a+1);
						if (b == string::npos) 
						{
							b = mText.find_first_of(" >", a+1);
							if (b == string::npos) return;
						}
						val = mText.substr(a+1, b-a-1);
					} 
					else if (mText[a] == '\'') 
					{
						string::size_type b = mText.find('\'', a+1);
						if (b == string::npos) 
						{
							b = mText.find_first_of(" >", a+1);
							if (b == string::npos) return;
						}
						val = mText.substr(a+1, b-a-1);
					} 
					else 
					{
						while (a < mText.length() && !isspace(mText[a]) && mText[a] != '>') 
						{
							val += mText[a++];
						}
					}

					mAttributes[key] = val;
				}
			}
		}

		bool Node::operator==(const Node &n) const 
		{
			if((!isTag()) || (!n.isTag())) return false;
			return !(strcasecmp(tagName().c_str(), n.tagName().c_str()));
		}


		ostream &operator<<(ostream &stream, const tree<HTML::Node> &tr) 
		{

			tree<HTML::Node>::pre_order_iterator it = tr.begin();
			tree<HTML::Node>::pre_order_iterator end = tr.end();

			int rootdepth = tr.depth(it);
			stream << "-----" << endl;

			uint n = 0;
			while ( it != end ) 
			{

				for(int i=0; i < tr.depth(it) - rootdepth; ++i) stream << "  ";
				stream << n << "@";
				stream << "[" << it->offset() << ";";
				stream << it->offset() + it->length() << ") ";
				stream << (string)(*it) << endl;
				++it, ++n;
			}
			stream << "-----" << endl;
			return stream;
		}
		Node::operator string() const {
			if (isTag()) return this->tagName();
			return this->text();
		}
		ostream &Node::operator<<(ostream &stream) const {
			stream << (string)(*this);
			return stream;
		}
	}//namespace html
}//namespace htmlcxx
