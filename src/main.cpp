#include <iostream>
#include "wrestd.h"
#include <thread>
#include "readprocess.h"
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include <stdio.h>

using namespace wrestd::io;

// Info of the song:
// all numbers are in fractional seconds
// "[songname]SONG_PART_DELIM[artist]SONG_PART_DELIM[album]SONG_PART_DELIM[duration]SONG_PART_DELIM[position]" is returned.
// duration will be the total seconds, position = how many seconds into the song you are
const char* songInfoCommand = "osascript -e 'tell app \"itunes\" to set theInfo to {name, artist, album, duration} of current track & {player position}' -e \"set AppleScript's text item delimiters to \\\"SONG_PART_DELIM\\\"\" -e 'set theInfoString to theInfo as string'";

int main(void) {
	std::string info;
	int child_stdout = -1;
	pid_t child_pid = openChildProc(songInfoCommand, 0, &child_stdout);
	if (!child_pid)
		std::cout << "A thing went wrong D:\n";
	else {
		char buff[5000];
		if (readProcessOut(child_stdout, buff, sizeof(buff)))
			info = std::string(buff);
			//std::cout << buff; // no EOL here, there is no need. output from commands should have an EOL at the end.
		else
			std::cout << "A different thing went wrong D:\n";
	}

	std::cout << info << std::endl;
	
	// name, artist, album, duration, player position
	std::vector<std::string> songparts;
	
	// split the string into the vector above by finding each delimiter
	std::string delimiter = "SONG_PART_DELIM";
	size_t pos = 0;
	std::string token;
	while ((pos = info.find(delimiter)) != std::string::npos) {
		token = info.substr(0, pos);
		songparts.push_back(token);
		info.erase(0, pos + delimiter.length());
	}
	songparts.push_back(info); // this gets the very last element

	// print out a top bar
	std::string bar(termWidth(), ' '); // filled with spaces
	// centered itti
	int nameIndex = (termWidth()/2)-2;
	bar.replace(nameIndex, 4, "itti");
	
	clear();

	printlc(bar, YELLOW, BLACK);

	// set precision to a fixed 2 decimal places
	std::cout.precision(2);
	std::cout << std::fixed;
	
	// for debug purposes, show the info
	std::cout << "Name:     " << songparts[0] << std::endl;
	std::cout << "Artist:   " << songparts[1] << std::endl;
	std::cout << "Album:    " << songparts[2] << std::endl;
	std::cout << "Duration: " << songparts[3] << std::endl;
	std::cout << "Position: " << songparts[4] << std::endl;
	
	// figure out how far along the song is
	double p = (std::stod(songparts[4])/std::stod(songparts[3]))*100.0;
	std::cout << "Percent:  " << p << std::endl;

	// for testing, at the moment.
	std::cout << "Height: " << termHeight() << " rows.\n";
	std::cout << "Width:  " << termWidth() << " columns.\n";
	

	return 0;
}
