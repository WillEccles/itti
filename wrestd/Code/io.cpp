#include "wrestd.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <fstream>
#include <cstdio>

// this is here because on VS2010 I have to change how this works to make it work properly
#define TO_STRING(i) (to_string(i))

// windows defined max, and I want to use a different one later on, meaning I have to undef this one
#ifdef _WIN32
#undef max
#endif

using namespace std;
using namespace wrestd::io;

const char* nixcolorcodes[16] = { "30", "34", "32", "36", "31", "35", "33", "37", "30;1", "34;1", "32;1", "36;1", "31;1", "35;1", "33;1", "37;1" };

// return the ANSI color code corresponding to the color_t specified
char* nixcolor(color_t color) {
	char *ncolor = (char *)malloc(15); // 15 is a wee bit large but it should be enough for all colors required
	if (color != DEFAULT)
		sprintf(ncolor, "\033[%sm", nixcolorcodes[color]);
	else // the person wants to set it to DEFAULT, which should be ESC[m
		sprintf(ncolor, "\033[m");
	return ncolor;
}

#ifdef _WIN32
// this will let me access the console
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

/* Set color of console text. */
void wrestd::io::setColor(color_t colorCode) {
#ifdef _WIN32
	// this will only work on windows
	SetConsoleTextAttribute(console, colorCode);
#else
	// here I will use the nixcolors above
	cout << nixcolor(colorCode);
#endif
}

/* Clear the console. */
void wrestd::io::clear() {
#ifdef _WIN32
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	FillConsoleOutputAttribute(console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
	SetConsoleCursorPosition(console, topLeft);
#else
	// this is the non-windows way of doing this
	// J clears the display, 2 means to clear the whole thing, and then cursor gets put at the top left
	cout << "\033[2J";
#endif
}

/* Print a line in a certain color. */
void wrestd::io::printlc(string line, color_t color) {
	setColor(color);
	cout << line << endl;
	setColor(DEFAULT);
}

/* Print in a certain color, but no newline. */
void wrestd::io::printc(string text, color_t color) {
	setColor(color);
	cout << text;
	setColor(DEFAULT);
}

/* Wait for user to press enter, optionally printing it in a specific color. */
void wrestd::io::wait(color_t messagecolor = DEFAULT) {
	setColor(messagecolor);
	cout << "\nPress ENTER to continue...";
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	setColor(DEFAULT);
}

/* Wait for user to press enter, display custom message, optionally specifying a color for the message. */
void wrestd::io::wait(string message, color_t messagecolor = DEFAULT) {
	setColor(messagecolor);
	cout << "\n" << message;
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	setColor(DEFAULT);
}

/* Return true or false based on the existance of a file. */
bool wrestd::io::fileExists(char filename[]) {
	if (ifstream(filename))
		return true;
	
	ofstream file(filename);
	if (!file) {
		file.close();
		return false;
	}

	return false; // just in case both of the above fail to work. assume no file.
}
