#include "html_parser.h"
#include "html_utils.h"
#include "config.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

#include <getopt.h>

using namespace std;
using namespace htmlcxx;

void usage(string prg) {
	cerr << "usage:\t" << prg << " [-h] [-V] file.html" << endl;
	return;
}

void usage_long(string prg) {
	usage(prg);
	cerr << "Html and css parser" << endl << endl;
	cerr << "  -V\t print version number and exit" << endl;
	cerr << "  -h\t print this help text" << endl;
	return;
}

int main(int argc, char **argv) {


	try {
		while(1) {
			char c = getopt(argc, argv, "hV");	
			if(c == -1) break;
			switch(c) {
				case 'h':
					usage_long(argv[0]);
					exit(0);
					break;
				case 'V':
					cerr << VERSION << endl;
					exit(0);
				default:
					usage(argv[0]);
					exit(1);
					break;
			}
		}

		if(argc != optind + 1) {
			usage(argv[0]);
			exit(1);
		}

		ifstream file(argv[optind]);
		if(!file.is_open()) {
			cerr << "Unable to open file " << argv[optind] << endl;
			exit(1);
		}
		string html;

		while(1) {
			char buf[BUFSIZ];
			file.read(buf, BUFSIZ);
			if(file.gcount() == 0) {
				break;
			}
			html.append(buf, file.gcount());
		}

		html::parser parser;
		tree<html::node> tr = parser.parse(html);
		cout << tr << endl;

	} catch (exception &e) {
		cerr << "Exception " << e.what() << " caught" << endl;
		exit(1);
	} catch (...) {
		cerr << "Unknow exception caught " << endl;
	}

	exit(0);
}
