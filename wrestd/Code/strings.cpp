#include "wrestd.h"
#include <string>

/* Count occurrences of substring 'substr' in std::string 'str'. */
int wrestd::strings::substr_count(std::string str, std::string substr, bool overlap=true) {
	if (str.find(substr) == str.npos) return 0;

	int index = 0;
	int count = 0;

	while (str.find(substr, index) != str.npos) {
		index = str.find(substr, index) + (overlap ? 1 : substr.length());
		count++;
	}

	return count;
}

/* Same as substr_replace, but acts directly on 'str'. 
Not available publicly, since it would be more confusing and I can just use the replace option of substr_replace. */
void substr_replace_volatile(std::string &str, std::string substr, std::string replacement, bool overlap = false) {
	if (str.find(substr) == str.npos) return;

	int index = 0;

	while (str.find(substr, index) != str.npos) {
		index = str.find(substr, index);
		str.erase(index, substr.length());
		str.insert(index, replacement);
		index += (overlap ? 1 : replacement.length());
	}
}

/* Replace all occurrences of substring 'substr' in std::string 'str'. */
std::string wrestd::strings::substr_replace(std::string &str, std::string substr, std::string replacement, bool overlap = false, bool replace = false) {
	if (replace) {
		substr_replace_volatile(str, substr, replacement, overlap);
		return str;
	}

	std::string _str = str;

	if (_str.find(substr) == _str.npos) return _str;

	int index = 0;

	while (_str.find(substr, index) != _str.npos) {
		index = _str.find(substr, index);
		_str.erase(index, substr.length());
		_str.insert(index, replacement);
		index += (overlap ? 1 : replacement.length());
	}

	return _str;
}

/* Convert a std::string to all lowercase. (ASCII-only obviously) */
std::string wrestd::strings::to_lower(std::string str) {
	std::string _str = str;
	
	for (int i = 0; i < _str.length(); i++)
		if (_str[i] >= 0x41/*A*/ && _str[i] <= 0x5A/*Z*/)
			_str[i] = (char)(_str[i] + 0x20);
			
	return _str;
}

/* Convert a std::string to all uppercase. (ASCII-only obviously) */
std::string wrestd::strings::to_upper(std::string str) {
	std::string _str = str;
	
	for (int i = 0; i < _str.length(); i++)
		if (_str[i] >= 0x61/*a*/ && _str[i] <= 0x7A/*z*/)
			_str[i] = (char)(_str[i] - 0x20);
			
	return _str;
}
