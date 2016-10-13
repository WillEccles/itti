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

// the following color code arrays are in the order of color_t color codes for reasy switching: color_t::black = 0, so nixcolorcodes[0] = 30, aka black
// foreground color codes for ANSI escapes
const char* nixcolorcodes[16] = { "30", "34", "32", "36", "31", "35", "33", "37", "30;1", "34;1", "32;1", "36;1", "31;1", "35;1", "33;1", "37;1" };
// background color codes for ANSI escapes
const char* nixbgcolorcodes[16] = { "40", "44", "42", "46", "41", "45", "43", "47", "40;1", "44;1", "42;1", "46;1", "41;1", "45;1", "43;1", "47;1" };

// return the ANSI color code corresponding to the color_t specified
char* nixcolor(color_t color) {
	char *ncolor = (char *)malloc(15); // 15 is a wee bit large but it should be enough for all colors required
	if (color != DEFAULT)
		sprintf(ncolor, "\033[%sm", nixcolorcodes[color]);
	else // the person wants to set it to DEFAULT, which should be ESC[m
		sprintf(ncolor, "\033[m");
	return ncolor;
}

// return the ANSI code for the background color_t specified
char* nixbgcolor(color_t color) {
	char *ncolor = (char *)malloc(15);
	if (color != DEFAULT)
		sprintf(ncolor, "\033[%sm", nixbgcolorcodes[color]);
	else
		sprintf(ncolor, "\033[m");
	return ncolor;
}

#ifdef _WIN32
// this will let me access the console
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

/* Set color of console text. Can specify background color if the system supports it. */
void wrestd::io::setColor(color_t colorCode, color_t bgColorCode = NULL) {
#ifdef _WIN32
	// this will only work on windows
	// if there is no bgColorCode specified, just do this
	if (bgColorCode == NULL)
		SetConsoleTextAttribute(console, colorCode);
	else {
		WORD color = ((bgColorCode & 0x0F) << 4) + (colorCode & 0x0F);
		SetConsoleTextAttribute(console, color);
	}
#else
	// here I will use the nixcolors above
	cout << nixcolor(colorCode);
	// if bgColorCode is specified, change the background to that too
	if (bgColorCode != NULL)
		cout << nixbgcolor(bgColorCode);
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

/* Print a line in a certain color. 
 * Optionally, specify a background color for systems that support it. */
void wrestd::io::printlc(string line, color_t color, color_t bgColorCode = NULL) {
	setColor(color, bgColorCode);
	cout << line << endl;
	setColor(DEFAULT, DEFAULT);
}

/* Print in a certain color, but no newline.
 * Optionally, specify a background color for systems that support it. */
void wrestd::io::printc(string text, color_t color, color_t bgColorCode = NULL) {
	setColor(color, bgColorCode);
	cout << text;
	setColor(DEFAULT, DEFAULT);
}

/* Wait for user to press enter, optionally printing it in a specific color. 
 * Also optionally add a background color shown on systems which support it. */
void wrestd::io::wait(color_t messagecolor = DEFAULT, color_t bgColor = NULL) {
	setColor(messagecolor, bgColor);
	cout << "\nPress ENTER to continue...";
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	setColor(DEFAULT, DEFAULT);
}

/* Wait for user to press enter, display custom message, optionally specifying a color for the message. 
 * Optionally specify background color for supported systems. */
void wrestd::io::wait(string message, color_t messagecolor = DEFAULT, color_t bgColorCode = NULL) {
	setColor(messagecolor, bgColorCode);
	cout << "\n" << message;
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	setColor(DEFAULT, DEFAULT);
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
