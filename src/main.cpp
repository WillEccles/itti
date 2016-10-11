#include <iostream>

// for color info see here: https://rosettacode.org/wiki/Terminal_control/Coloured_text

int main(void) {
	std::cout << "\033[30;47m" << "this should be cool" << "\033[m" << std::endl;
	return 0;
}
