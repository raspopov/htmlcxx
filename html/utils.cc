#include <algorithm>
#include <cctype>
#include <regexx.hh>
#include <strstream>
#include "utils.h"

using namespace std;
namespace htmlcxx {
	namespace HTML {

		string single_blank(const string &str) {

			string ret;	
			uint i;
			ret.reserve(str.length());

			// Skip space in the beginning
			i = 0;
			while (isspace(str[i])) ++i;

			uint length = str.length();
			uint last_is_ispace = false;
			for(; i < length; ++i) {
				if(isspace(str[i])) {
					if(last_is_ispace) continue;
					last_is_ispace = true;
				} else {
					if (last_is_ispace) ret += " ";
					last_is_ispace = false;
					ret += str[i];
				}
			}

			ret.resize(ret.size());

			return ret;

		}

		string strip_comments(const string &str) {

			string ret;
			ret.reserve(str.size());

			const char *ptr = str.c_str();
			const char *end = ptr + str.length();

			bool inside_comment = false;
			while(1) {
				if(!inside_comment) {
					if(ptr  + 4 < end) {
						if(*ptr == '<' && *(ptr+1) == '!' && *(ptr+2) =='-' && *(ptr + 3) == '-' && isspace(*(ptr + 4))) {
							inside_comment = true;
						}
					}
				} else {
					if(ptr + 2 < end) {
						if(*ptr == '-' && *(ptr+1) == '-' && *(ptr+2) == '>' ) {
							inside_comment = false;
							ptr += 3;
						}
					}
				}
				if(ptr == end) break;
				if(!inside_comment) ret += *ptr;
				ptr++;
			}

			ret.resize(ret.size());

			return ret;

		}



		static struct {
			char *str;
			char chr;
		} entities[] = {
			/* 00 */
			{ "quot", 34 },
			{ "amp", 38 },
			{ "lt", 60 },
			{ "gt", 62 },
			{ "nbsp", ' ' },
			{ "iexcl", 161 },
			{ "cent", 162 },
			{ "pound", 163 },
			{ "curren", 164 },
			{ "yen", 165 },
			/* 10 */
			{ "brvbar", 166 },
			{ "sect", 167 },
			{ "uml", 168 },
			{ "copy", 169 },
			{ "ordf", 170 },
			{ "laquo", 171 },
			{ "not", 172 },
			{ "shy", 173 },
			{ "reg", 174 },
			{ "macr", 175 },
			/* 20 */
			{ "deg", 176 },
			{ "plusmn", 177 },
			{ "sup2", 178 },
			{ "sup3", 179 },
			{ "acute", 180 },
			{ "micro", 181 },
			{ "para", 182 },
			{ "middot", 183 },
			{ "cedil", 184 },
			{ "sup1", 185 },
			/* 30 */
			{ "ordm", 186 },
			{ "raquo", 187 },
			{ "frac14", 188 },
			{ "frac12", 189 },
			{ "frac34", 190 },
			{ "iquest", 191 },
			{ "Agrave", 192 },
			{ "Aacute", 193 },
			{ "Acirc", 194 },
			{ "Atilde", 195 },
			/* 40 */
			{ "Auml", 196 },
			{ "ring", 197 },
			{ "AElig", 198 },
			{ "Ccedil", 199 },
			{ "Egrave", 200 },
			{ "Eacute", 201 },
			{ "Ecirc", 202 },
			{ "Euml", 203 },
			{ "Igrave", 204 },
			{ "Iacute", 205 },
			/* 50 */
			{ "Icirc", 206 },
			{ "Iuml", 207 },
			{ "ETH", 208 },
			{ "Ntilde", 209 },
			{ "Ograve", 210 },
			{ "Oacute", 211 },
			{ "Ocirc", 212 },
			{ "Otilde", 213 },
			{ "Ouml", 214 },
			{ "times", 215 },
			/* 60 */
			{ "Oslash", 216 },
			{ "Ugrave", 217 },
			{ "Uacute", 218 },
			{ "Ucirc", 219 },
			{ "Uuml", 220 },
			{ "Yacute", 221 },
			{ "THORN", 222 },
			{ "szlig", 223 },
			{ "agrave", 224 },
			{ "aacute", 225 },
			/* 70 */
			{ "acirc", 226 },
			{ "atilde", 227 },
			{ "auml", 228 },
			{ "aring", 229 },
			{ "aelig", 230 },
			{ "ccedil", 231 },
			{ "egrave", 232 },
			{ "eacute", 233 },
			{ "ecirc", 234 },
			{ "euml", 235 },
			/* 80 */
			{ "igrave", 236 },
			{ "iacute", 237 },
			{ "icirc", 238 },
			{ "iuml", 239 },
			{ "ieth", 240 },
			{ "ntilde", 241 },
			{ "ograve", 242 },
			{ "oacute", 243 },
			{ "ocirc", 244 },
			{ "otilde", 245 },
			/* 90 */
			{ "ouml", 246 },
			{ "divide", 247 },
			{ "oslash", 248 },
			{ "ugrave", 249 },
			{ "uacute", 250 },
			{ "ucirc", 251 },
			{ "uuml", 252 },
			{ "yacute", 253 },
			{ "thorn", 254 },
			{ "yuml", 255 },
			/* 100 */
			{ NULL, 0 },
		};

		string decode_entities(const string &s) {

			string str(s);
			size_t init = 0;
			size_t end = 0;
			string entity;

			//		printf("url_init: %s\n", str.c_str());
			while (1) {

				init = str.find("&", end);
				if (init == string::npos) break;
				end = str.find(";", init);
				if (end == string::npos) break;

				entity = str.substr(init + 1, end - init - 1);
				//			printf("entity: %s\n", entity.c_str());
				if (!entity.empty() && entity[0] == '#') {
					entity.erase(0, 1);
					unsigned char chr = atoi(entity.c_str());
					if (chr != 0) {
						str.replace(init, end - init + 1, string(1, chr));
						end = init+1;
					} else {
						str.replace(init, end - init + 1, string(""));
						end = init;
					}
				} else {
					for (int i = 0; entities[i].str != NULL; i++) {
						if (entity == entities[i].str) {
							str.replace(init, end - init + 1, string(1, entities[i].chr));
							end = init+1;
							break;
						}
					}
				}
			}

			//		printf("url_end: %s\n", str.c_str());
			return str;
		}

		string get_attribute(const string& tag, const string& attr) {
			string val;
			string low_tag(tag);
			string low_attr(attr);

			transform(low_attr.begin(), low_attr.end(), low_attr.begin(), ::tolower);
			transform(low_tag.begin(), low_tag.end(), low_tag.begin(), ::tolower);

			string::size_type a;
			a = low_tag.find(low_attr);
			if (a == string::npos)
				return val;

			a += attr.length();
			while (a < tag.length() && isspace(tag[a])) a++;
			if (a == tag.length() || tag[a] != '=')
				return val;
			a++;
			while (a < tag.length() && isspace(tag[a])) a++;
			if (a == tag.length())
				return val;

			if (tag[a] == '"') {
				string::size_type b = tag.find('"', a+1);
				if (b == string::npos) return val;
				val = tag.substr(a+1, b-a-1);
			} else if (tag[a] == '\'') {
				string::size_type b = tag.find('\'', a+1);
				if (b == string::npos) return val;
				val = tag.substr(a+1, b-a-1);
			} else {
				while (a < tag.length() && !isspace(tag[a]) && tag[a] != '>') {
					val += tag[a++];
				}
			}

			return val;
		}

		string convert_link(const string& relative, const string& root) {

			string url(relative);
			size_t find;

			url = HTML::decode_entities(url);

			if ((find = url.find("#")) != string::npos) url.erase(find);

			if (url.find("http://") == 0) return url;
			if (url.find("https://") == 0) return url;
			if (url.find("javascript:") == 0) return url;
			if (url.find("mailto:") == 0) return url;
			if (url.find("ftp://") == 0) return url;
			if (url.find("gopher://") == 0) return url;
			if (url.find("file:") == 0) url.erase(0, strlen("file:"));

			int absolute = 0;
			if (url[0] == '/') absolute = 1;

			static regexx::Regexx hostpat;
			hostpat.expr("(\\w+)://(([\\w:-]+)@)?([\\w\\.-]+)(:(\\d+))?(/.*)?");
			if (hostpat.str(root).exec() == 0) return url;
			string protocol = hostpat.match[0].atom[0].str();
			string host = hostpat.match[0].atom[3].str();
			if (hostpat.match[0].atom.size() > 4)
				host += hostpat.match[0].atom[4].str();
			string path;
			if (hostpat.match[0].atom.size() > 6)
				path = hostpat.match[0].atom[6].str();
			string prefix(protocol + string("://"));

			if (path.empty()) path = "/";

			if (absolute) {
				path = url;
			} else {
				if ((find = path.find("?")) != string::npos) path.erase(find);
				find = path.rfind("/");
				if (find != string::npos) path.erase(find+1);
				path += url;
			}

			while ((find = path.find("//")) != string::npos) path.erase(find, 1);
			while ((find = path.find("/./")) != string::npos) path.erase(find, 2);
			while ((find = path.find("/../")) != string::npos) {
				size_t dir_to_erase;
				dir_to_erase = path.rfind("/", find - 1);
				if (dir_to_erase >= find) dir_to_erase = string::npos;
				if (dir_to_erase != string::npos) {
					path.erase(dir_to_erase, find - dir_to_erase + 3);
				} else {
					path.erase(find, 3);
				}
			}

			url = prefix + host + path;

			string::size_type a;
			while ((a = url.find("\r")) != string::npos) {
				url.erase(a, 1);
			}
			while ((a = url.find("\n")) != string::npos) {
				url.erase(a, 1);
			}

			return url;
		}

		string __serialize_gml(const tree<HTML::Node> &tr, tree<HTML::Node>::iterator it, tree<HTML::Node>::iterator end, uint parent_id, uint& label) {

			using namespace std;
			ostrstream ret;
			tree<HTML::Node>::sibling_iterator sib = tr.begin(it);
			while(sib != tr.end(it)) {
				ret << "node [ id " << ++label << "\n label \"" << label << "\"\n]\n";
				ret << "edge [ \n source " << parent_id << "\n target " << label << "\n]" << endl;
				ret << __serialize_gml(tr, sib, end, label, label);
				++sib;
			}	
			ret << ends;
			string str = ret.str();
			ret.freeze(0);
			return str;
		}


		string serialize_gml(const tree<HTML::Node> &tr) {

			using namespace std;

			tree<HTML::Node>::pre_order_iterator it = tr.begin();
			tree<HTML::Node>::pre_order_iterator end = tr.end();

			string ret;
			ret += "graph [";
			ret += "directed 1\n";
			ret += "node [ id 0\n label \"0\"\n ]\n";
			uint label = 0;
			ret += __serialize_gml(tr, it, end, 0, label);
			ret += "]";
			return ret;

		}

	}//namespace html
}//namespace htmlcxx
