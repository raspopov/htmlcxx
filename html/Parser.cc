#include "Parser.h"

#include <iostream>
#include <strstream>

//#define DEBUG
#include "debug.h"

#define TAG_NAME_MAX 10

using namespace std;
using namespace htmlcxx; 
using namespace HTML; 

static
struct literal_tag {
	int len;
	char* str;
	int is_cdata;
}   
literal_mode_elem[] =
{   
	{6, "script", 1},
	{5, "style", 1},
	{3, "xmp", 1},
	{9, "plaintext", 1},
	{8, "textarea", 0},
	{0, 0, 0}
};


const tree<HTML::Node> &Parser::parse(const string &html) 
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

	char const *b = c;

	while (c < end)
	{
		// For some tags, the text inside it is considered literal and is only
		// close for its </TAG> counterpart
		while (mpLiteral)
		{
			DEBUGP("Treating literal %s\n", mpLiteral);
			while (c < end && *c != '<') ++c;

			if (c == end) {
				if (c != b) this->parseContent(b, c, end);
				goto DONE;
			}
			
			const char *end_text = c;
			++c;

			if (*c == '/')
			{
				++c;
				const char *l = mpLiteral;
				while (*l && ::tolower(*c) == *l)
				{
					++c;
					++l;
				}

				if (!*l && strcmp(mpLiteral, "plaintext"))
				{
					// matched all and is not tag plain text
					while (isspace(*c)) ++c;

					if (*c == '>')
					{
						++c;
						if (b != end_text)
							this->parseContent(b, end_text, end);
						mpLiteral = 0;
						c = end_text;
						b = c;
						continue;
					}
				}
			}
			else if (*c == '!')
			{
				// we may find a comment and we should support it
				const char *e;
				e = c + 1;

				if (e < end && *e == '-' && e+1 < end && *(e+1) == '-')
				{
					DEBUGP("Parsing comment\n");
					e += 2;
					c = this->skipHtmlComment(e, end);
				}

//				if (b != end_text)
//					this->parseContent(b, end_text, end);

//				this->parseComment(end_text, c, end);

				// continue from the end of the comment
//				b = c;
			}
		}

		if (*c == '<')
		{
			const char *d = c + 1;;
			if (d < end && *d != 0)
			{
				if (isalpha(*d))
				{
					// beginning of tag
					if (b != c)
						this->parseContent(b, c, end);

					DEBUGP("Parsing beginning of tag\n");
					this->parseHtmlTag(d, end);

					// continue from the end of the tag
					c = d;
					b = c;
					continue;
				}

				if (*d == '/')
				{
					if (b != c)
						this->parseContent(b, c, end);

					if (d+1 < end && isalpha(*(d+1)))
					{
						// end of tag
						DEBUGP("Parsing end of tag\n");
						this->parseHtmlTag(d, end);
					}
					else
					{
						// not a conforming end of tag, treat as comment
						DEBUGP("Non conforming end of tag\n");
						d = this->skipHtmlTag(d, end);
						this->parseComment(c, d, end);
					}

					// continue from the end of the tag
					c = d;
					b = c;
					continue;
				}

				if (*d == '!')
				{
					// comment
					if (b != c)
						this->parseContent(b, c, end);

					const char *e;
					e = d + 1;

					if (e < end && *e == '-' && e+1 < end && *(e+1) == '-')
					{
						DEBUGP("Parsing comment\n");
						e += 2;
						d = this->skipHtmlComment(e, end);
					}
					else
					{
						d = this->skipHtmlTag(d, end);
					}

					this->parseComment(c, d, end);

					// continue from the end of the comment
					c = d;
					b = c;
					continue;
				}

				if (*d == '?' || *d == '%')
				{
					// something like <?xml or <%VBSCRIPT
					if (b != c)
						this->parseContent(b, c, end);

					d = this->skipHtmlTag(d, end);

					this->parseComment(c, d, end);

					// continue from the end of the comment
					c = d;
					b = c;
					continue;
				}
			}
		}
		c++;
		//print_tree(mHtmlTree, mHtmlTree.begin(), mHtmlTree.end());
	}

	// There may be some text in the end of the document
	if (c != b)
	{
		this->parseContent(b, c, end);
	}

DONE:
	//Correct never closing tags limits
	//FIXME if <BODY> never closes?
#ifdef DEBUG
	cerr << mHtmlTree << endl;
#endif
	return mHtmlTree;
}

void Parser::parseComment(char const *b, char const *c, char const *end)
{
	DEBUGP("Creating comment node %s\n", (string(b, c - b)).c_str());
	HTML::Node com_node;
	//FIXME: set_tagname shouldn't be needed, but first I must check
	//legacy code
	string comment(b, c - b);
	com_node.tagName(comment);
	com_node.text(comment);
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
	string text(b, c - b);
	txt_node.tagName(text);
	txt_node.text(text);
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
	c = this->skipHtmlTag(c, end);

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

		int tag_len = strlen(name);
		for (int i = 0; literal_mode_elem[i].len; ++i)
		{
			if (tag_len == literal_mode_elem[i].len)
			{
				if (!strcasecmp(name, literal_mode_elem[i].str))
				{
					mpLiteral = literal_mode_elem[i].str;
					break;
				}
			}
		}
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
			cerr << "comparing " << name + 1 << " with " << i->tagName()<<endl<<":";
			if (!i->tagName().length()) cerr << "Tag with no name at" << i->offset()<<";"<<i->offset()+i->length();
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
//			HTML::Node txt_node;
//			txt_node.offset((uint)(tag_begin - mpStart));
			//by now, length is just the size of the tag
//			txt_node.length(tag_end - tag_begin);
//			txt_node.text(string(tag_begin - 1, tag_end - tag_begin + 2));
//			txt_node.isTag(false);
//			txt_node.isComment(false);

			//This is here only for backward compatibility.
			//Using get_tagname with a text node is deprecated
//			txt_node.tagName(string(tag_begin, tag_end - tag_begin));
//			mHtmlTree.append_child(mCurrentState, txt_node);

			HTML::Node tag_node;
			tag_node.offset((uint)(tag_begin - mpStart));
			//by now, length is just the size of the tag
			tag_node.length(tag_end - tag_begin);
			tag_node.text(string(tag_begin - 1, tag_end - tag_begin + 2));
			tag_node.isTag(false);
			tag_node.isComment(true);
			tag_node.tagName(string(tag_begin, tag_end - tag_begin));
			mHtmlTree.append_child(mCurrentState, tag_node);
		}

	}

	free(name);
}

const char *Parser::skipHtmlComment(char const *ptr, char const *end)
{
	const char *c = ptr;
	while ( c != end ) {
		if ( *c++ == '-' && c != end && *c == '-' ) {
			char const *const d = c;
			while ( ++c != end && isspace( *c ) );
			if ( c == end || *c++ == '>' ) break;
			c = d;
		}
	}

	return c;
}

const char *Parser::skipHtmlTag(char const *ptr, char const *end)
{
	const char *c = ptr;
	while (c != end && *c != '>')
	{
		if (*c != '=') 
		{
			++c;
		}
		else
		{ // found an attribute
			++c;
			while (c != end && isspace(*c)) ++c;

			if (c == end) break;

			if (*c == '\"' || *c == '\'') 
			{
				char quote = *c++;
				const char *save = c;
//				while (c != end && *c != quote) ++c;
				c = static_cast<char*>(memchr(c, quote, end - c));
				if (c != NULL) 
				{
					++c;
				} 
				else 
				{
					c = save;
				}
				DEBUGP("Quotes: %s\n", (string(save - 1, c - save  + 1)).c_str());
			}
		}
	}

	if (c != end) ++c;
	
	return c;
}

ostream &HTML::operator<<(ostream &stream, const tree<HTML::Node> &tr) 
{

	tree<HTML::Node>::pre_order_iterator it = tr.begin();
	tree<HTML::Node>::pre_order_iterator end = tr.end();

	int rootdepth = tr.depth(it);
	stream << "-----" << endl;

	uint n = 0;
	while ( it != end ) 
	{

		int cur_depth = tr.depth(it);
		for(int i=0; i < cur_depth - rootdepth; ++i) stream << "  ";
		stream << n << "@";
		stream << "[" << it->offset() << ";";
		stream << it->offset() + it->length() << ") ";
		stream << (string)(*it) << endl;
		++it, ++n;
	}
	stream << "-----" << endl;
	return stream;
}
