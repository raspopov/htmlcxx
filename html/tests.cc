#include "parser.h"
#include "utils.h"
#include <string>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace htmlcxx;

#define myassert(x) \
	do {\
		if(!(x)) {\
			fprintf(stderr, "Test at %s:%d failed!\n", __FILE__, __LINE__);\
			exit(1);\
		}\
	} while(0)

class HtmlTest {
	public:

	bool parse() {
		cerr << "Parsing some html... ";
		tree<HTML::Node> tr;
		string html = "<head></head><body>\n\n\n\n<center>\n<table width=\"600\">\n<tbody><tr>\n<td width=\"120\"><a href=\"/index.html\"><img src=\"/adt-SUA/images/ADT_LOGO.gif\" alt=\"adt logo\" align=\"middle\" border=\"0\"></a></td>\n<td width=\"480\"><font size=\"+2\" face=\"helvetica,arial\"><b>Australian Digital Theses Program<br></b></font></td>\n</tr>\n</tbody></table>\n</center>\n<center>\n</center>\n</body>";

		HTML::Parser parser;
		tr = parser.parse(html);
		cerr << tr << endl;
		cerr << " ok" << endl;
		return true;
	}

	bool string_manip() {

		string comments = "hello <!-- world --> brazil";
		string multiblank = "1 2  3   4    5     6";
		myassert(HTML::strip_comments(comments) == "hello  brazil");
		myassert(HTML::single_blank(multiblank) == "1 2 3 4 5 6");

		return true;
	}
};


int main(int argc, char **argv) {

	HtmlTest ht;
	myassert(ht.parse());
	myassert(ht.string_manip());

	return 0;
}
