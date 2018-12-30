#pragma once

#include <set>

using namespace std;

extern const int CHAR_ANY;
extern const int CHAR_WORD;
extern const int CHAR_STRING_START;
extern const int CHAR_STRING_END;

class Charset {
public:
	set<int> chars;
	int specialChar = 0;

	Charset() {}

	Charset(set<int> && chars) :chars(chars) {}

	inline bool matchSpecial(const int &ch) const {
		if (specialChar == CHAR_ANY)
			return true;
		else if (specialChar == CHAR_WORD) { // [a-zA-Z0-9_]
			return ((int)'a' <= ch && ch <= (int)'z') ||
				((int)'A' <= ch && ch <= (int)'Z') ||
				((int)'0' <= ch && ch <= (int)'9') ||
				(ch == (int)'_');
		}
		else if (specialChar == CHAR_STRING_END) {
			return ch < 0;
		}
		else return specialChar == ch;
	}

	inline bool match(const int &ch) const {
		if (specialChar != 0)
			return matchSpecial(ch);
		return chars.find(ch) != chars.end();
	}
};