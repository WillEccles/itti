#include <iostream>
#include "wrestd.h"
#include <thread>
#include "readprocess.h" // getting song info
#include <string>
#include <regex>
#include <vector>
#include <iomanip> // for manipulating iostreams
#include <stdio.h>
#include <unistd.h> // sleep(seconds)
#include <ncurses.h> // this should make the development easier
#include <signal.h>
#include <algorithm>
#include "itunescommands.h" // itunes commands
#include <time.h> // nanosleep(nanoseconds), nonbusy sleep
#include <chrono> // for time to sleep in std::this_thread::sleep_for()

// color pairs
#define BAR_PAIR 1
#define RESET_PAIR 2
#define GENERAL_PAIR 3
#define DEFAULT_PAIR RESET_PAIR // same thing, but makes it look better in some cases to use this

// convert MS to NS
#define MILLIS(m) m * 1000000
// sleep in terms of milliseconds
#define MSLEEP(ms) nanosleep(MILLIS(ms))

// how often to refresh information
#define REFRESH_INTERVAL 1s

short fg_color, bg_color; // store colors
bool isResizing = false; // whether or not the display is resizing
bool willQuit = false; // when this is set to true, helper threads will end and can be joined

// note: must keep in mind that LINES and COLS exist
#define ROWS LINES // ROWS/COLS seems more reasonable to me, alias ROWS to LINES

// prints song info after the titles that are placed in printTitles()
//
void printSongInfo(std::string title, std::string album, std::string artist) {
	/*
	  Title: [title]
	 Artist: [artist]
	  Album: [album]
	 */
	
	// get the max length of a line
	int maxlen = COLS - 10 - 2; // this is the number of columns minus the two spaces on either end and the heading
	
	// TODO: add an ellipsis when the string is too long to show all of it
	
	// now I just have to print this information.
	
	std::string blank(maxlen, ' ');
	
	// find out what rows to print on
	int r1 = ROWS/2-1;
	mvwaddnstr(stdscr, r1, 10, std::string(maxlen, ' ').replace(0, title.length(), title).c_str(), maxlen);
	mvwaddnstr(stdscr, r1+1, 10, std::string(maxlen, ' ').replace(0, artist.length(), artist).c_str(), maxlen);
	mvwaddnstr(stdscr, r1+2, 10, std::string(maxlen, ' ').replace(0, album.length(), album).c_str(), maxlen);
}

// prints the title bar and titles for song information
void printTitles() {
	std::string title = "";
	for (int i = 0; i < COLS; i++)
		title += " ";
	
	// add the "itti" in the middle (or as close to the middle as possible)
	title.replace(COLS/2-2, 4, "itti");
	
	attron(A_BOLD);
	attron(COLOR_PAIR(BAR_PAIR));
	mvwaddstr(stdscr, 0, 0, title.c_str());
	attroff(COLOR_PAIR(BAR_PAIR));
	
	// now we can print the headings for song info
	char titleheading[12];
	sprintf(titleheading, "%10s", "Title: ");
	char artistheading[12];
	sprintf(artistheading, "%10s", "Artist: ");
	char albumheading[12];
	sprintf(albumheading, "%10s", "Album: ");
	
	// print the headings for title, artist, etc.
	int rowNum0 = ROWS/2-1;
	mvwaddstr(stdscr, rowNum0, 0, titleheading);
	mvwaddstr(stdscr, rowNum0+1, 0, artistheading);
	mvwaddstr(stdscr, rowNum0+2, 0, albumheading);
	
	attroff(A_BOLD);
	refresh();
}

// this handles when the display is resized
void handleResize(int sig) {
	endwin();
	// Needs to be called after an endwin() so ncurses will initialize
	// itself with the new terminal dimensions.
	refresh();
	clear();
	printTitles();
}

// currentTime = player progress
// totalTime = song duration
// both are in seconds
// returns something like "-01:23" which means there is 1 minute 23 seconds left
std::string timeStr(double currentTime, double totalTime) {
	double timeRemaining = totalTime - currentTime; // this is the time we want to show
	
	int timeMinutes = (int)timeRemaining/60;
	int timeHours = timeMinutes/60;
	timeMinutes = timeMinutes%60;
	int timeSeconds = (int)timeRemaining%60;
	
	char timeString[10];
	
	if (timeHours > 0) {
		sprintf(timeString, "-%02d:%02d:%02d", timeHours, timeMinutes, timeSeconds);
	}
	else if (timeMinutes > 0) {
		sprintf(timeString, "-%02d:%02d", timeMinutes, timeSeconds);
	}
	else {
		sprintf(timeString, "-0:%02d", timeSeconds);
	}
	
	return std::string(timeString);
}

// this is run by the thread in charge of updating the display
void updateDisplay() {
	for (;;) {
		if (willQuit) return;
		if (!isResizing) {
			// this stuff grabs the song info
			std::string info = commandOutput(SONG_INFO);
			if (info != PROCESS_ERROR && info != OUTPUT_ERROR && info.length() > 0) {
				// name, artist, album, duration, player position, sound volume
				std::vector<std::string> songparts;
				
				// store the stuff there
				std::string delimiter = "SONG_PART_DELIM";
				size_t pos = 0;
				std::string token;
				while ((pos = info.find(delimiter)) != std::string::npos) {
					token = info.substr(0, pos);
					songparts.push_back(token);
					info.erase(0, pos + delimiter.length());
				}
				songparts.push_back(info); // this gets the very last element
				
				char volStr[10];
				sprintf(volStr, " vol: %d%%", std::stoi(songparts[5]));
				std::string volume(volStr);
				
				std::string tStr = timeStr(std::stod(songparts[4]), std::stod(songparts[3]));
				
				// get the length of the progress bar, with 2 spaces of padding on each end + the time string and the two brackets to put on it
				// also subtract the length of the volume string from the end
				size_t barLen = COLS - 4 - tStr.length() - 1 - 2 - volume.length();
				// 4 = padding
				// 1 = space between time and bar
				// 2 = for the brackets
				
				// set up the progress bar
				std::string progBar;
				size_t p = size_t( (std::stod(songparts[4])/std::stod(songparts[3])) * (double)barLen );
				progBar = "  " + tStr + " [" + std::string(p, '#') + std::string(barLen - p, '-') + "]" + volume + "  ";
				
				// print the title and whatnot
				printSongInfo(songparts[0], songparts[2], songparts[1]);
				
				// print it
				attron(COLOR_PAIR(BAR_PAIR));
				mvwaddstr(stdscr, ROWS-1, 0, progBar.c_str());
				attroff(COLOR_PAIR(BAR_PAIR));
				
				refresh();
				
				// this is a good way of sleeping a single thread
				// 1s is a c++14 addition, it's the same as std::chrono::seconds(1) [see REFRESH_INTERVAL above]
				std::this_thread::sleep_for(REFRESH_INTERVAL);
			} else if (info == PROCESS_ERROR) {
				// there was an error starting the process
				std::cout << "Error starting process [line 188]" << std::endl;
			} else if (info == OUTPUT_ERROR) {
				// there was an error getting output from process's stdout
				//std::cout << "Error getting output from process [line 191]" << std::endl;
			}
		}
	}
}

// changes the volume of the player, if -5 is specified it lowers it
void changeVol(int amt = 5) {
	// first get the player's volume
	std::string info = commandOutput(PLAYER_VOLUME);
	if (info != PROCESS_ERROR && info != OUTPUT_ERROR) {
		// first determine the sound volume as an integer
		int currentVolume = std::stoi(info);
		system(SET_PLAYER_VOLUME(currentVolume + amt));
	} else if (info == PROCESS_ERROR) {
		std::cout << PROCESS_ERROR << std::endl;
		// error running process
	} else if (info == OUTPUT_ERROR) {
		std::cout << OUTPUT_ERROR << std::endl;
		// error getting output from process
	}
}

#define increaseVolume() changeVol()
#define decreaseVolume() changeVol(-5)

int main(void) {
	// initialize the screen
	initscr();
	start_color();
	noecho();
	cbreak();
	curs_set(0); // 0, 1, and 2 are the visibilities for the cursor, so 0 is "not there"
	
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = handleResize;
	sigaction(SIGWINCH, &sa, NULL);
	
	// color pairs
	init_pair(BAR_PAIR, COLOR_WHITE, COLOR_BLUE); // white on yellow
	init_pair(RESET_PAIR, -1, -1);
	init_pair(GENERAL_PAIR, -1, -1);
	
	refresh();
	
	// draw the top bar
	printTitles();
	
	std::thread t(updateDisplay);
	
	// detaches the thread. we don't need to use std::thread::join() since this means it will kill itself when it's done.
	//t.detach();
	
	/*
	 Proposed Controls:
	 q - quit
	 p - play/pause
	 s - stop
	 < - prev
	 > - next
	 ] or = - vol up
	 [ or - - vol down
	 */
	
	bool inputLoop = true;
	int key = -1;
	
	// I have decided to leave the system() calls because in this case I am not worried too much about malicious replacement of osascript
	while (inputLoop && (key = getch())) {
		// if the key is a key
		switch(key) {
			case 113: // q
				inputLoop = false;
				break;
			case 112:
				system(PLAYER_TOGGLE);
				break;
			case 115:
				system(PLAYER_STOP);
				break;
			case 60: // <
				system(PLAYER_PREV);
				break;
			case 62: // >
				system(PLAYER_NEXT);
				break;
			case 61: // = (aka the + key), falls through to ]
			case 93: // ]
				increaseVolume();
				break;
			case 45: // - (falls through to [)
			case 91: // [
				decreaseVolume();
				break;
			default:
				break;
		}
	}
	willQuit = true; // kills the other threads
	
	t.join(); //we don't need this line, since we use std::thread::detach above, which means that the thread will keep executing until it's done
	// this could be dangerous, so we have to make sure willQuit is true, otherwise the thread will be orphaned
	
	endwin();
	
	return 0;
}
