#pragma once

#include <set>

using namespace std;

extern const int CHAR_ANY;
extern const int CHAR_WORD;
extern const int CHAR_STRING_START;
extern const int CHAR_STRING_END;


inline int toLowerCase(int ch) {
	if (65 <= ch && ch <= 90)
		return ch + 32;
	return ch;
}

inline int toUpperCase(int ch) {
	if (97 <= ch && ch <= 122)
		return ch - 32;
	return ch;
}

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

	template<bool case_sensitive>
	inline bool match(const int &ch) const {
		if (specialChar != 0)
			return matchSpecial(ch);
		if (case_sensitive)
			return chars.find(ch) != chars.end();
		else {
			auto upper = toUpperCase(ch);
			auto lower = toLowerCase(ch);
			bool findU = chars.find(upper) != chars.end();
			bool findL = chars.find(lower) != chars.end();
			return findU || findL;
		}
	}
};

