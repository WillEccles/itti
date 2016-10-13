#include "wrestd.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <fstream>
#include <cstdio>

// to get properties of the terminal on BSD, etc.
#ifndef _WIN32
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#endif

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
const int nixcolorcodes[16] = { 30, 34, 32, 36, 31, 35, 33, 37, 90, 94, 92, 96, 91, 95, 93, 97 };
// background color codes for ANSI escapes
const int nixbgcolorcodes[16] = { 40, 44, 42, 46, 41, 45, 43, 47, 100, 104, 102, 106, 101, 105, 103, 107 };

int getFGColor(int color) {
	if (color == DEFAULT)
		return NOFG;
	else return nixcolorcodes[color];
}

int getBGColor(int color) {
	if (color == DEFAULT)
		return NOBG;
	else return nixbgcolorcodes[color];
}

#ifdef _WIN32
// this will let me access the console
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

/* Set color of console text. Can specify background color if the system supports it. 
 *
 * FOR BEST RESULTS ON NON-WINDOWS:
 * DO NOT USE THIS FOR BACKGROUND COLOR
 * INSTEAD, SPECIFY ONE FOR PRINTLC OR PRINTC
 * */
void wrestd::io::setColor(color_t colorCode, color_t bgColorCode) {
#ifdef _WIN32
	// this will only work on windows
	// if there is no bgColorCode specified, just do this
	if (bgColorCode == NOBG)
		SetConsoleTextAttribute(console, colorCode);
	else {
		WORD color = ((bgColorCode & 0x0F) << 4) + (colorCode & 0x0F);
		SetConsoleTextAttribute(console, color);
	}
#else
	// here I will use the nixcolors above
	int fgcolor = getFGColor(colorCode);
	int bgcolor = getBGColor(bgColorCode);
	printf("\033[%d;%dm", fgcolor, bgcolor);
#endif
}

/* Set the format of the text to the specified format_t.
 * Non-Windows only ATM. */
void wrestd::io::setFormat(format_t fmt) {
#ifndef _WIN32
	printf("\033[%dm", fmt);
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
	printf("\33c\033[2J");
#endif
}

/* Print a line in a certain color. 
 * Optionally, specify a background color for systems that support it. */
void wrestd::io::printlc(string line, color_t color, color_t bgColorCode ) {
#ifdef _WIN32
	setColor(color, bgColorCode);
	cout << line << endl;
	setColor(DEFAULT, DEFAULT);
#else
	printf("\033[%d;%dm%s\033[m\n", getFGColor(color), getBGColor(bgColorCode), line.c_str());
#endif
}

/* Print in a certain color, but no newline.
 * Optionally, specify a background color for systems that support it. */
void wrestd::io::printc(string text, color_t color, color_t bgColorCode ) {
#ifdef _WIN32
	setColor(color, bgColorCode);
	cout << text;
	setColor(DEFAULT, DEFAULT);
#else
	printf("\033[%d;%dm%s\033[m", getFGColor(color), getBGColor(bgColorCode), text.c_str());
#endif
}

/* Wait for user to press enter, optionally printing it in a specific color. 
 * Also optionally add a background color shown on systems which support it. */
void wrestd::io::wait(color_t messagecolor, color_t bgColor ) {
#ifdef _WIN32
	setColor(messagecolor, bgColor);
	cout << "\nPress ENTER to continue...";
#else
	printf("\n\033[%d;%dmPress ENTER to continue...\033[m", getFGColor(messagecolor), getBGColor(bgColor));
#endif
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#ifdef _WIN32
	setColor(DEFAULT, DEFAULT);
#endif
}

/* Wait for user to press enter, display custom message, optionally specifying a color for the message. 
 * Optionally specify background color for supported systems. */
void wrestd::io::wait(string message, color_t messagecolor, color_t bgColorCode ) {
#ifdef _WIN32
	setColor(messagecolor, bgColorCode);
	cout << "\n" << message;
#else
	printf("\n\033[%d;%dm%s\033[m", getFGColor(messagecolor), getBGColor(bgColorCode), message.c_str());
#endif
	// this is why i had to undef max, because windows defines it in some header file somewhere
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#ifdef _WIN32
	setColor(DEFAULT, DEFAULT);
#endif
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

/* Retrieve the width of the terminal window in columns.
 * Returns -1 if unable to retrieve. */
int wrestd::io::termWidth() {
#ifndef _WIN32
	struct winsize ws;
	int fd;

	// open terminal
	fd = open("/dev/tty", O_RDWR);
	if (fd < 0)
		return -1;

	// get the size
	if (ioctl(fd, TIOCGWINSZ, &ws) < 0)
		return -1;

	// otherwise return the width
	return ws.ws_col;
#endif
}

/* Retrieve the height of the terminal window in rows.
 * Returns -1 if error. */
int wrestd::io::termHeight() {
#ifndef _WIN32
	struct winsize ws;
	int fd;

	// open terminal
	fd = open("/dev/tty", O_RDWR);
	if (fd < 0)
		return -1;

	// get size
	if (ioctl(fd, TIOCGWINSZ, &ws) < 0)
		return -1;

	// return height
	return ws.ws_row;
#endif
}
